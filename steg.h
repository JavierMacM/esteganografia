#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <fstream>
unsigned char MaskTable[] = 
{ 
0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 
};
//FUncion para obtener el desplazamiento en ASCII en el archvio imagen.bmp
int obtener_desplazamiento(FILE* image) {
  int desp;
  //(Archivo, desplazamiento,inicio del dezplazamiento)
	fseek(image,10,0);
  //Obtener el caracter en el desplazamiento y obtener numero ASCII
	desp=(int)fgetc(image);
	return desp;
}

//FUncion para regresar el tamaño del mensaje a ocultar
int obtener_size_mensaje(FILE *image) {
  int size = ftell(image);
  //Recorrer el mensaje a ocultar, con long int hasta el final del texto
	fseek(image, 0L, SEEK_END);
	fseek(image, 0L, SEEK_SET);
	return(size);
}


int obtener_bit(char byte,int bit) {
	return((byte>>8-bit)&1);
}
