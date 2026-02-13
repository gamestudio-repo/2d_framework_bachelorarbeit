struct Chunk{
	unsigned int length=0;
	byte chunktype[4]={0,0,0,0};
	byte* chunkdata=nullptr;
	byte crc[4]={0,0,0,0};
	
	void clear(){
		if(chunkdata!=nullptr){
			delete[] chunkdata;
			chunkdata=nullptr;
		}
	}
};

struct bitreader{
	max_uint bits_read=0;
	byte* bytestream=nullptr;
	
	void set(byte* stream){
		bytestream=stream;
	}
	
	void roundToByte(){
		bits_read+=(8-(bits_read%8));
	}
	
	max_uint read(max_uint bits){
		max_uint result=0;
		max_uint currentByte=0;
		byte read_bits=0;
		for(max_uint i=0;i<bits;i++){
			currentByte=bits_read/8;
			read_bits=bits_read%8;
			result|=((bytestream[currentByte]>>read_bits)&1)<<i;
			bits_read++;
		}
		return result;
	}
	
	void back(max_uint bits){
		bits_read-=bits;
	}
	
};

struct huffman_tree{
	
	max_uint max_length=0;
	max_uint min_length=0;
	max_uint size=0;
	unsigned int* codes=nullptr;
	byte* is_assign=nullptr;
	
	void create(byte* bitlengths,max_uint size){
		for(max_uint i=0;i<size;i++){
			if(max_length<bitlengths[i]){
				max_length=bitlengths[i];
			}
		}
		min_length=max_length;
		for(max_uint i=0;i<size;i++){
			if(min_length>bitlengths[i] && bitlengths[i]!=0){
				min_length=bitlengths[i];
			}
		}
		byte* code_length=new byte[max_length+1];
		for(max_uint i=0;i<max_length+1;i++){
			code_length[i]=0;
		}
		for(max_uint i=0;i<size;i++){
			code_length[bitlengths[i]]++;
		}
		code_length[0]=0;
		unsigned int code=0;
		unsigned int first_codes[max_length+1];
		for(max_uint i=0;i<max_length+1;i++){
			first_codes[i]=0;
		}
		for(max_uint i=1;i<max_length+1;i++){
			code=(code+code_length[i-1])<<1;
			if(code_length[i]>0){
				first_codes[i]=code;
			}
		}
		delete code_length;
		this->size=size;
		codes=new unsigned int[size];
		is_assign=new byte[size];
		for(max_uint i=0;i<size;i++){
			codes[i]=0;
			is_assign[i]=0;
		}
		for(max_uint i=0;i<size;i++){
			if(bitlengths[i]>0){
				codes[i]=first_codes[bitlengths[i]];
				first_codes[bitlengths[i]]++;
				is_assign[i]=bitlengths[i];
			}
		}
	}
	
	max_uint reverseBits(max_uint value,max_uint size){
		max_uint result=0;
		for(max_uint i=0;i<size;i++){
			result|=((value>>(size-(1+i))&1)<<i);
		}
		return result;
	}
	
	unsigned int decode(bitreader& reader){
		unsigned int bit_code=0;
		for(unsigned int i=min_length;i<=max_length;i++){
			bit_code=reverseBits(reader.read(i),i);
			for(unsigned int k=0;k<size;k++){
				if(bit_code==codes[k] && is_assign[k]>0 && is_assign[k]==i){
					return k;
				}
			}
			reader.back(i);
		}
		throw "Huffman Tree Error: Keine gültige Encodierung gefunden!";
	}
	
	void clear(){
		if(codes!=nullptr){
			delete[] codes;
			codes=nullptr;
		}
		if(is_assign!=nullptr){
			delete[] is_assign;
			is_assign=nullptr;
		}
		size=0;
	}
	
};

class PNGLoader{
	
	byte bit_order[19]={16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
	
	byte base_length_extra_bit[29] = {
		0, 0, 0, 0, 0, 0, 0, 0, //257 - 264
		1, 1, 1, 1, //265 - 268
		2, 2, 2, 2, //269 - 273 
		3, 3, 3, 3, //274 - 276
		4, 4, 4, 4, //278 - 280
		5, 5, 5, 5, //281 - 284
		0           //285
	};

	unsigned int base_lengths[29] = {
		3, 4, 5, 6, 7, 8, 9, 10, //257 - 264
		11, 13, 15, 17,          //265 - 268
		19, 23, 27, 31,          //269 - 273 
		35, 43, 51, 59,          //274 - 276
		67, 83, 99, 115,         //278 - 280
		131, 163, 195, 227,      //281 - 284
		258                      //285
	};
	
	unsigned int dist_bases[32] = {
		/*0*/ 1, 2, 3, 4,    //0-3
		/*1*/ 5, 7,          //4-5
		/*2*/ 9, 13,         //6-7
		/*3*/ 17, 25,        //8-9
		/*4*/ 33, 49,        //10-11
		/*5*/ 65, 97,        //12-13
		/*6*/ 129, 193,      //14-15
		/*7*/ 257, 385,      //16-17
		/*8*/ 513, 769,      //18-19
		/*9*/ 1025, 1537,    //20-21
		/*10*/ 2049, 3073,   //22-23
		/*11*/ 4097, 6145,   //24-25
		/*12*/ 8193, 12289,  //26-27
		/*13*/ 16385, 24577,  //28-29
			0   , 0      //30-31, error, shouldn't occur
	};

	unsigned int dist_extra_bits[32] = {
		/*0*/ 0, 0, 0, 0, //0-3
		/*1*/ 1, 1,       //4-5
		/*2*/ 2, 2,       //6-7
		/*3*/ 3, 3,       //8-9
		/*4*/ 4, 4,       //10-11
		/*5*/ 5, 5,       //12-13
		/*6*/ 6, 6,       //14-15
		/*7*/ 7, 7,       //16-17
		/*8*/ 8, 8,       //18-19
		/*9*/ 9, 9,       //20-21
		/*10*/ 10, 10,    //22-23
		/*11*/ 11, 11,    //24-25
		/*12*/ 12, 12,    //26-27
		/*13*/ 13, 13,     //28-29
			0 , 0      //30-31 error, they shouldn't occur
	};
	
	byte chunktypes[4][4]={	{73,72,68,82},	//IHDR
							{80,76,84,69},	//PLTE
							{73,69,78,68},	//IEND
							{73,68,65,84}};	//IDAT
	byte signature[8]={137,80,78,71,13,10,26,10};
	
	unsigned int width=0;
	unsigned int height=0;
	byte bitdepth=0;
	byte colortype=0;
	byte compression_method=0;
	byte filter_method=0;
	byte interlace_method=0;
	byte pixel_length=3;
	byte* pixelarray=nullptr;
	max_uint currentPos=0;	//Für IDAT um das Pixelarray zu erstellen
	
	
	bool checkSignature(PFile& file){
		for(int i=0;i<8;i++){
			if(file.readByte()!=signature[i]){
				return false;
			}
		}
		return true;
	}
	
	uint32_t adler32(byte* data,max_uint size) {
		const uint32_t MOD_ADLER = 65521;
		uint32_t a = 1, b = 0;
		for (max_uint i = 0; i < size; ++i) {
			a = (a + data[i]) % MOD_ADLER;
			b = (b + a) % MOD_ADLER;
		}
		return (b << 16) | a;
	}
	
	void chunkIHDR(byte* stream,max_uint size){
		width=	(stream[0]<<24)+(stream[1]<<16)+
				(stream[2]<<8)+(stream[3]);
		height=	(stream[4]<<24)+(stream[5]<<16)+
				(stream[6]<<8)+(stream[7]);
		bitdepth=stream[8];
		colortype=stream[9];
		compression_method=stream[10];
		filter_method=stream[11];
		interlace_method=stream[12];
		if(colortype==6){
			pixel_length=4;
		}
		pixelarray=new byte[(width*pixel_length+1)*height];
	}
	
	void chunkIDAT(byte* stream,max_uint size){
		deflate(stream,size);
	}
	
	void chunkPLTE(byte* stream,max_uint size){
		return;
	}
	
	void chunkIEND(byte* stream,max_uint size){
		return;
	}
	
	max_uint reverseBits(max_uint value,max_uint size){
		max_uint result=0;
		for(byte i=0;i<size;i++){
			result|=((value>>(size-(1+i))&1)<<i);
		}
		return result;
	}
	
	byte fixed_huffman_code_length(bitreader& reader){
		byte huffmanbytes[5];
		for(int i=0;i<5;i++){
			huffmanbytes[i]=reader.read(1);
		}
		reader.back(5);
		if(huffmanbytes[0]==1 && huffmanbytes[1]==1){
			if(huffmanbytes[2]==1 || huffmanbytes[3]==1 || huffmanbytes[4]==1){
				return 9;
			}
			return 8;
		}
		if(huffmanbytes[0]==0 && huffmanbytes[1]==0){
			if(huffmanbytes[2]==1 && huffmanbytes[3]==1){
				return 8;
			}
			return 7;
		}
		return 8;
	}
	
	unsigned int decode_fixed_huff_code(bitreader& reader,unsigned int huff_length){
		unsigned int result=reverseBits(reader.read(huff_length),huff_length);
		switch(huff_length){
			case 7:
				return result+256;
			case 8:
				if(result<192){
					return result-48;
				}
				return result+280-192;
			case 9:
				return result+144-400;
		}
		return 1000;
	}
	
	void no_compression(bitreader& reader){
		reader.roundToByte();
		unsigned int len=reader.read(8*2);
		unsigned int nlen=reader.read(8*2);
		for(unsigned int i=0;i<len;i++){
			pixelarray[currentPos++] = reader.read(8);
		}
	}
	
	void fixed_huffman(bitreader& reader){
		unsigned int decoded_value=0;
		unsigned int base_index = 0;
		unsigned int duplicate_length = 0;
		unsigned int distance_index = 0;
		unsigned int distance_length = 0;
		unsigned int back_pointer_index = 0;
		unsigned int huff_code_length=0;
		max_uint counter=0;
		while(true) {
			huff_code_length=fixed_huffman_code_length(reader);
			decoded_value = decode_fixed_huff_code(reader,huff_code_length);
			counter+=huff_code_length;
			if(decoded_value == 256) break;
			if(decoded_value < 256) { //its a literal so just output it
				pixelarray[currentPos++] = decoded_value;
				continue;
			}
			if(decoded_value < 286 && decoded_value > 256) {
				base_index = decoded_value - 257;
				duplicate_length = base_lengths[base_index] + reader.read(base_length_extra_bit[base_index]);
				counter+=base_length_extra_bit[base_index];

				distance_index = reverseBits(reader.read(5),5);
				distance_length = dist_bases[distance_index] + reader.read(dist_extra_bits[distance_index]);
				counter+=dist_extra_bits[distance_index];
				
				back_pointer_index = currentPos - distance_length;
				while(duplicate_length--) {
					pixelarray[currentPos++] = pixelarray[back_pointer_index++];
				}
			}
		}
	}
	
	void dynamic_huffman(bitreader& reader){
		unsigned int hlit=reader.read(5)+257;
		byte hdist=reader.read(5)+1;
		byte hclen=reader.read(4)+4;
		byte code_lengths[19];
		for(byte i=0;i<19;i++){
			code_lengths[i]=0;
		}
		for(byte i=0;i<hclen;i++){
			code_lengths[bit_order[i]]=reader.read(3);
		}
		huffman_tree code_tree;
		code_tree.create(code_lengths,19);
		byte* code_length_dist=new byte[hlit+hdist];
		unsigned int counter=0;
		unsigned int decoded_value=0;
		unsigned int repeat_count = 0;
		byte code_length_to_repeat = 0;
		unsigned int base_index = 0;
		unsigned int duplicate_length = 0;
		unsigned int distance_index = 0;
		unsigned int distance_length = 0;
		unsigned int back_pointer_index = 0;
		while(counter<(hdist+hlit)){
			decoded_value = code_tree.decode(reader);
			if(decoded_value < 16) {
				code_length_dist[counter++] = decoded_value;
				continue;
			}
			repeat_count = 0;
			code_length_to_repeat = 0;
			switch(decoded_value) {
				case 16:
					repeat_count = reader.read(2) + 3;// 3 - 6 repeat count
					code_length_to_repeat = code_length_dist[counter - 1];
					break;
				case 17:
					repeat_count = reader.read(3) + 3;// 3 - 10 repeat count
					break;
				case 18:
					repeat_count = reader.read(7) + 11;// 3 - 10 repeat count
					break;
			}
			for(unsigned int i=0;i<repeat_count;i++){
				code_length_dist[counter+i]=code_length_to_repeat;
			}
			counter += repeat_count;
		}
		huffman_tree length_tree;
		huffman_tree distance_tree;
		length_tree.create(code_length_dist,hlit);
		distance_tree.create(code_length_dist+hlit,hdist);
		while(true) {
			decoded_value = length_tree.decode(reader);
			if(decoded_value == 256) break;
			if(decoded_value < 256) { //its a literal so just output it
				pixelarray[currentPos++] = decoded_value;
				continue;
			}

			if(decoded_value < 286 && decoded_value > 256) {
				base_index = decoded_value - 257;
				duplicate_length = base_lengths[base_index] + reader.read(base_length_extra_bit[base_index]);;

				distance_index = distance_tree.decode(reader);
				distance_length = dist_bases[distance_index] + reader.read(dist_extra_bits[distance_index]);

				back_pointer_index = currentPos - distance_length;
				while(duplicate_length--) {
					pixelarray[currentPos++] = pixelarray[back_pointer_index++];
				}

			}
		}
		delete[] code_length_dist;
		code_tree.clear();
		length_tree.clear();
		distance_tree.clear();
	}
	
	void deflate(byte* stream,max_uint size){
		bitreader reader;
		reader.set(stream);
		byte compression_info=reader.read(8);
		byte flag=reader.read(8);
		byte counter=2;
		if((flag>>5)&1){
			reader.read(8*4);
			counter+=4;
		}
		byte finalblock=0;
		byte compression=0;
		while(finalblock==0){
			finalblock=reader.read(1);
			compression=reader.read(2);
			switch(compression){
				case 0:
					no_compression(reader);
					break;
				case 1:
					fixed_huffman(reader);
					break;
				case 2:
					dynamic_huffman(reader);
					break;
				case 3:
					throw "Unbekannte Kompression in der PNG!";
					break;
			}
		}
		unsigned int adler_32 =		(stream[size - 4] << 24) |
									(stream[size - 3] << 16) |
									(stream[size - 2] << 8) |
									stream[size - 1];
		unsigned int adler_32_check = adler32(stream+counter,size-(4+counter));
		delete[] stream;
		stream=nullptr;
	}
	
	void createPixels(){
		byte* pixels=new byte[(width*pixel_length)*height];
		for(max_uint i=0;i<height;i++){
			for(max_uint k=0;k<width;k++){
				for(max_uint l=0;l<pixel_length;l++){
					pixels[(width*pixel_length)*i+k*pixel_length+l]=pixelarray[(width*pixel_length+1)*i+1+k*pixel_length+l];
				}
			}
		}
		byte filtermethod=0;
		for(max_uint i=0;i<height;i++){
			filtermethod=pixelarray[i*(width*pixel_length+1)];
			switch(filtermethod){
				case 0:
					break;
				case 1:
					for(max_uint k=1;k<width;k++){
						for(byte l=0;l<pixel_length;l++){
							pixels[(width*pixel_length)*i+k*pixel_length+l]=pixels[(width*pixel_length)*i+(k-1)*pixel_length+l]+pixels[(width*pixel_length)*i+k*pixel_length+l];
						}
					}
					break;
				case 2:
					for(max_uint k=0;k<width;k++){
						for(byte l=0;l<pixel_length;l++){
							pixels[(width*pixel_length)*i+k*pixel_length+l]=(pixels[(width*pixel_length)*(i-1)+k*pixel_length+l]+pixels[(width*pixel_length)*i+k*pixel_length+l]);
						}
					}
					break;
				case 3:
					for(byte l=0;l<pixel_length;l++){
						pixels[(width*pixel_length)*i+l]=(pixels[(width*pixel_length)*(i-1)+l]>>1)+pixels[(width*pixel_length)*i+l];
					}
					for(int k=1;k<width;k++){
						for(byte l=0;l<pixel_length;l++){
							pixels[(width*pixel_length)*i+k*pixel_length+l]=((pixels[(width*pixel_length)*i+(k-1)*pixel_length+l]+pixels[(width*pixel_length)*(i-1)+k*pixel_length+l])>>1)+pixels[(width*pixel_length)*i+k*pixel_length+l];
						}
					}
					break;
				case 4:
					for(byte l=0;l<pixel_length;l++){
						pixels[(width*pixel_length)*i+l]=pixels[(width*pixel_length)*(i-1)+l]+pixels[(width*pixel_length)*i+l];
					}
					for(int k=1;k<width;k++){
						for(byte l=0;l<pixel_length;l++){
							pixels[(width*pixel_length)*i+k*pixel_length+l]=path_predict(pixels[(width*pixel_length)*i+(k-1)*pixel_length+l],pixels[(width*pixel_length)*(i-1)+k*pixel_length+l],pixels[(width*pixel_length)*(i-1)+(k-1)*pixel_length+l])+pixels[(width*pixel_length)*i+k*pixel_length+l];
						}
					}
					break;
			}
		}
		if(pixelarray!=nullptr){
			delete[] pixelarray;
		}
		pixelarray=new byte[(width*pixel_length)*height];
		for(max_uint i=0;i<(width*pixel_length)*height;i++){
			pixelarray[i]=pixels[i];
		}
		delete[] pixels;
	}
	
	void readChunkList(LinkedList<Chunk>& list,LinkedList<Chunk>& idatchunks,PFile& file){
		while(true){
			Chunk newchunk;
			newchunk.length=(file.readByte()<<24)+
							(file.readByte()<<16)+
							(file.readByte()<<8)+
							(file.readByte());
			for(int i=0;i<4;i++){
				newchunk.chunktype[i]=file.readByte();
			}
			newchunk.chunkdata=new byte[newchunk.length];
			for(max_uint i=0;i<newchunk.length;i++){
				newchunk.chunkdata[i]=file.readByte();
			}
			for(int i=0;i<4;i++){
				newchunk.crc[i]=file.readByte();
			}
			if(newchunk.chunktype[0]==chunktypes[3][0] && newchunk.chunktype[1]==chunktypes[3][1] &&
				newchunk.chunktype[2]==chunktypes[3][2] && newchunk.chunktype[3]==chunktypes[3][3]){
				idatchunks.add(newchunk);
			}
			else{
				list.add(newchunk);
			}
			if(chunktypes[2][0]==newchunk.chunktype[0]&&
				chunktypes[2][1]==newchunk.chunktype[1]&&
				chunktypes[2][2]==newchunk.chunktype[2]&&
				chunktypes[2][3]==newchunk.chunktype[3]){
				return;
			}
		}
	}
	
	void handleChunks(LinkedList<Chunk>& list){
		byte chunktype=0;
		LinkedList<Chunk>* currentlist=&list;
		bool haselement=currentlist->hasElement();
		Chunk chunk;
		while(haselement){
			chunk=currentlist->element();
			for(byte i=0;i<3;i++){
				chunktype=i+1;
				for(byte k=0;k<4;k++){
					if(chunk.chunktype[k]!=chunktypes[i][k]){
						chunktype=0;
						break;
					}
				}
				if(chunktype>0){
					break;
				}
			}
			switch(chunktype){
				case 1:
					chunkIHDR(chunk.chunkdata,chunk.length);
					break;
				case 2:
					chunkPLTE(chunk.chunkdata,chunk.length);
					break;
				case 3:
					chunkIEND(chunk.chunkdata,chunk.length);
					break;
			}
			currentlist=currentlist->next();
			if(currentlist==nullptr){
				break;
			}
			haselement=currentlist->hasElement();
		}
	}
	
	void get_idat_stream(byte*& stream,LinkedList<Chunk>& idatchunks,max_uint& size){
		max_uint streamsize=0;
		max_uint currentPos=0;
		LinkedList<Chunk>* currentlist=&idatchunks;
		bool haselement=currentlist->hasElement();
		Chunk chunk;
		while(haselement){
			chunk=currentlist->element();
			streamsize+=chunk.length;
			currentlist=currentlist->next();
			if(currentlist==nullptr){
				break;
			}
			haselement=currentlist->hasElement();
		}
		stream=new byte[streamsize];
		size=streamsize;
		currentlist=&idatchunks;
		haselement=currentlist->hasElement();
		while(haselement){
			chunk=currentlist->element();
			for(max_uint i=0;i<chunk.length;i++){
				stream[currentPos]=chunk.chunkdata[i];
				currentPos++;
			}
			currentlist=currentlist->next();
			if(currentlist==nullptr){
				break;
			}
			haselement=currentlist->hasElement();
		}
	}
	
	void clearChunkList(LinkedList<Chunk>& list){
		LinkedList<Chunk>* currentlist=&list;
		while(true){
			if(currentlist==nullptr){
				break;
			}
			if(currentlist->hasElement()){
				(currentlist->element()).clear();
			}
			currentlist=currentlist->next();
		}
		list.clear();
	}
	
	int betrag(int value){
		if(value>=0){
			return value;
		}
		return (-1)*value;
	}
	
	int path_predict(int a,int b,int c){
		int p = a + b - c;
		int pa = betrag(p - a);
		int pb = betrag(p - b);
		int pc = betrag(p - c);
		if(pa <= pb && pa <= pc){
			return a;
		}
		if(pb <= pc){
			return b;
		}
		return c;
	}
	
	public:
	
	void reset(){
		width=0;
		height=0;
		bitdepth=0;
		colortype=0;
		compression_method=0;
		filter_method=0;
		interlace_method=0;
		pixel_length=3;
		if(pixelarray!=nullptr){
			delete[] pixelarray;
		}
		pixelarray=nullptr;
		currentPos=0;
	}
	
	bool hasTransparency(){
		if(pixel_length==4){
			return true;
		}
		return false;
	}
	
	byte* getPixels(){
		return pixelarray;
	}
	
	max_uint getWidth(){
		return width;
	}
	
	max_uint getHeight(){
		return height;
	}
	
	byte getPixelLength(){
		return pixel_length;
	}
	
	bool load(const char* string){
		PFile file;
		if(file.load(string,1)){
			if(!checkSignature(file)){
				return false;
			}
			LinkedList<Chunk> chunklist;
			LinkedList<Chunk> idatchunks;
			readChunkList(chunklist,idatchunks,file);
			byte* idatstream=nullptr;
			max_uint idatsize=0;
			get_idat_stream(idatstream,idatchunks,idatsize);
			file.close();
			handleChunks(chunklist);
			deflate(idatstream,idatsize);
			createPixels();
			clearChunkList(chunklist);
			clearChunkList(idatchunks);
			return true;
		}
		return false;
	}
	
	~PNGLoader(){
		if(pixelarray!=nullptr){
			delete[] pixelarray;
		}
	}
	
};