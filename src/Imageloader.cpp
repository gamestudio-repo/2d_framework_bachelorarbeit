class Imageloader{
	
	const static byte supported_type_amount=2;
	const char* supported_types[supported_type_amount]={"bmp","png"};

	byte getType(const char* stringarray){
		max_uint size=stringLength(stringarray);
		max_uint type_index_start=0;
		for(max_uint i=1;i<=size;i++){
			if(stringarray[size-i]=='.'){
				type_index_start=(size-i)+1;		// (size-i) damit man '.' indiziert und die +1 damit der index auf datentype ist
				break;
			}
		}
		if(type_index_start==0){
			return 0;
		}
		byte typesize=size-type_index_start;
		char type[typesize+1];
		for(byte i=0;i<typesize;i++){
			type[i]=stringarray[type_index_start+i];
		}
		type[typesize]='\0';
		string typestring=type;
		for(byte i=0;i<supported_type_amount;i++){
			if(typestring==supported_types[i]){
				return i+1;
			}
		}
		return 0;
	}
	
	

	template<typename Loadertype,typename Objecttype>
	Objecttype* loadImage(const char* stringarray,byte imagetype){
		Loadertype loader;
		loader.load(stringarray);
		Objecttype* object=new Objecttype();
		if(imagetype==0){
			object->loadPixels(loader.getPixels(),loader.getWidth(),loader.getHeight());
			return object;
		}
		int loaderwidth=loader.getWidth();
		int loaderheight=loader.getHeight();
		uint8_t* changed_pixels=new uint8_t[loaderwidth*loaderheight*3];
		uint8_t* trans_pixels=new uint8_t[loaderwidth*loaderheight];
		uint8_t* loader_pixels=loader.getPixels();
		byte loader_pixel_length=loader.getPixelLength();
		for(int i=0;i<loaderheight;i++){
			for(int j=0;j<loaderwidth;j++){
				for(int k=0;k<3;k++){
					changed_pixels[loaderwidth*3*(loaderheight-(i+1))+j*3+(2-k)]=loader_pixels[i*loaderwidth*loader_pixel_length+j*loader_pixel_length+k];
				}
			}
		}
		for(int i=0;i<loaderheight;i++){
			for(int j=0;j<loaderwidth;j++){
				trans_pixels[loaderwidth*(loaderheight-(i+1))+j]=loader_pixels[i*loaderwidth*loader_pixel_length+j*loader_pixel_length+(loader_pixel_length-1)];
			}
		}
		object->loadPixels(changed_pixels,loaderwidth,loaderheight,trans_pixels);
		delete[] changed_pixels;
		delete[] trans_pixels;
		return object;
	}
	
	template<typename Objecttype>
	Objecttype* errorMessage(const char* stringarray){
		string errorstring="Imageloader Error: Dateityp von '";
		errorstring+=stringarray;
		errorstring+="' wird nicht erkannt!";
		throw errorstring;
	}

	public:
	
	template<typename Objecttype>
	Objecttype* load(const char* stringarray){
		byte type_id=getType(stringarray);
		string loadarray="Graphics/";
		loadarray+=stringarray;
		switch(type_id){
			case 0:
				return errorMessage<Objecttype>(loadarray);
			case 1:
				return loadImage<BMPLoader,Objecttype>(loadarray,0);
			case 2:
				return loadImage<PNGLoader,Objecttype>(loadarray,1);
		}
		return nullptr;
	}
	
};