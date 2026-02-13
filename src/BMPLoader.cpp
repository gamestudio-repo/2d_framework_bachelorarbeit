class BMPLoader{
	BITMAPINFO bitmapinfo;
	max_uint width=0;
	max_uint height=0;
	uint8_t* pixels=nullptr;
	byte pixel_length=3;

	void resetBitmapInfo(){
		bitmapinfo.bmiHeader.biSize=0;
		bitmapinfo.bmiHeader.biWidth=0;
		bitmapinfo.bmiHeader.biHeight=0;
		bitmapinfo.bmiHeader.biPlanes=0;
		bitmapinfo.bmiHeader.biBitCount=0;
		bitmapinfo.bmiHeader.biCompression=0;
		bitmapinfo.bmiHeader.biSizeImage=0;
		bitmapinfo.bmiHeader.biXPelsPerMeter=0;
		bitmapinfo.bmiHeader.biYPelsPerMeter=0;
		bitmapinfo.bmiHeader.biClrUsed=0;
		bitmapinfo.bmiHeader.biClrImportant=0;
	}
	
	public:
	
	BITMAPINFO* getBitmapInfo(){
		return &bitmapinfo;
	}
	
	bool hasTransparency(){
		return false;
	}
	
	uint8_t* getPixels(){
		return pixels;
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
	
	void reset(){
		resetBitmapInfo();
		width=0;
		height=0;
		if(pixels!=nullptr){
			delete[] pixels;
			pixels=nullptr;
		}
	}

	byte load(const char* realpath){
		byte bmpcheck[2]={0x42,0x4D};
		int reserved[2];
		long long offset=0;
		long long size=0;
		long long infoheadersize=0;
		int biplanes=0;
		int bibitcount=0;
		int bicompression=0;
		long long bisizeimage=0;
		long long biclrused=0;
		long long biclrimportant=0;
		long long bixpelspermeter=0;
		long long biypelspermeter=0;
		std::ifstream file;
		file.open(realpath,std::ifstream::in | std::ifstream::binary);
		if(file.is_open()){
			file.seekg(0, std::ios::end);
			size=file.tellg();
			int position=0;
			char* buffer=new char[size];
			buffer[size]='\0';
			file.seekg(0, std::ios::beg);
			file.read(buffer,size);
			file.close();
			if(((byte)buffer[position]!=bmpcheck[0])||((byte)buffer[position+1]!=bmpcheck[1])){
				std::cout<<"Keine gueltige Bitmap Datei!"<<"\n";
				return 0;
			}
			position+=2;
			size =((long long)((byte)buffer[position]))+
				  ((long long)((byte)buffer[position+1])<<8)+
				  ((long long)((byte)buffer[position+2])<<16)+
				  ((long long)(byte)buffer[position+3]<<24);
			position+=4;
			reserved[0]=(int)((byte)buffer[position]<<8)|((byte)buffer[position+1]);
			reserved[1]=(int)((byte)buffer[position+2]<<8)|((byte)buffer[position+3]);
			position+=4;
			offset =((long long)((byte)buffer[position]))+
					((long long)((byte)buffer[position+1])<<8)+
					((long long)((byte)buffer[position+2])<<16)+
					((long long)(byte)buffer[position+3]<<24);
			position+=4;
			infoheadersize=((long long)((byte)buffer[position])<<24)+
						   ((long long)((byte)buffer[position+1])<<16)+
						   ((long long)((byte)buffer[position+2])<<8)+
						   ((long long)(byte)buffer[position+3]);
			position+=4;
			width	=	((long long)((byte)buffer[position+3])<<24)+
						((long long)((byte)buffer[position+2])<<16)+
						((long long)((byte)buffer[position+1])<<8)+
						((long long)(byte)buffer[position]);
			position+=4;
			height	=	((long long)((byte)buffer[position+3])<<24)+
						((long long)((byte)buffer[position+2])<<16)+
						((long long)((byte)buffer[position+1])<<8)+
						((long long)(byte)buffer[position]);
			position+=4;
			biplanes   =((long long)((byte)buffer[position]))+
						((long long)((byte)buffer[position+1]));
			position+=2;
			bibitcount =((long long)((byte)buffer[position]))+
						((long long)((byte)buffer[position+1]));
			position+=2;
			bicompression=((long long)((byte)buffer[position]))+
						  ((long long)((byte)buffer[position+1]))+
						  ((long long)((byte)buffer[position+2]))+
						  ((long long)((byte)buffer[position+3]));
			position+=4;
			bisizeimage	= ((long long)((byte)buffer[position+3])<<24)+
						  ((long long)((byte)buffer[position+2])<<16)+
						  ((long long)((byte)buffer[position+1])<<8)+
						  ((long long)(byte)buffer[position]);
			position+=4;
			bixpelspermeter = ((long long)((byte)buffer[position+3])<<24)+
							  ((long long)((byte)buffer[position+2])<<16)+
							  ((long long)((byte)buffer[position+1])<<8)+
							  ((long long)(byte)buffer[position]);
			position+=4;
			biypelspermeter = ((long long)((byte)buffer[position+3])<<24)+
							  ((long long)((byte)buffer[position+2])<<16)+
							  ((long long)((byte)buffer[position+1])<<8)+
							  ((long long)(byte)buffer[position]);
			position+=4;
			biclrused = ((long long)((byte)buffer[position+3])<<24)+
						((long long)((byte)buffer[position+2])<<16)+
						((long long)((byte)buffer[position+1])<<8)+
						((long long)(byte)buffer[position]);
			position+=4;
			biclrimportant =  ((long long)((byte)buffer[position+3])<<24)+
							  ((long long)((byte)buffer[position+2])<<16)+
							  ((long long)((byte)buffer[position+1])<<8)+
							  ((long long)(byte)buffer[position]);
			position=offset;
			switch(bibitcount){
				case 32:
					pixels=new uint8_t[height*width*4];
					for(int i=0;i<height;i++){
						for(int j=0;j<width;j++){
							for(byte k=0;k<4;k++){
								pixels[i*width*3+j*3+k]=(byte)buffer[position+i*width*3+j*3+k];
							}
						}
					}
					break;
				case 24:
					int bitmapwidth=((width*3+3)/4)*4;
					pixels=new uint8_t[height*width*3];
					for(int i=0;i<height;i++){
						for(int j=0;j<width;j++){
							for(byte k=0;k<3;k++){
								pixels[i*width*3+j*3+k]=(byte)buffer[position+i*bitmapwidth+j*3+k];
							}
						}
					}
					break;
			}
			delete[] buffer;
			bitmapinfo.bmiHeader.biSize=sizeof(bitmapinfo.bmiHeader);
			bitmapinfo.bmiHeader.biWidth=width;
			bitmapinfo.bmiHeader.biHeight=height;
			bitmapinfo.bmiHeader.biPlanes=biplanes;
			bitmapinfo.bmiHeader.biBitCount=bibitcount;
			bitmapinfo.bmiHeader.biCompression=0;
			bitmapinfo.bmiHeader.biSizeImage=bisizeimage;
			bitmapinfo.bmiHeader.biXPelsPerMeter=bixpelspermeter;
			bitmapinfo.bmiHeader.biYPelsPerMeter=biypelspermeter;
			bitmapinfo.bmiHeader.biClrUsed=biclrused;
			bitmapinfo.bmiHeader.biClrImportant=biclrimportant;
			return 1;
		}
		file.close();
		return 0;
	}
	
	BMPLoader(){}
	
	~BMPLoader(){
		if(pixels!=nullptr){
			delete[] pixels;
		}
	}
};