/*---------------------------------------
* Seguridad Informática: Proyecto Final
*
* Fecha: 03/06/2020
*
* Autor: Irving Alain Aguilar Pérez - A1703171
* 		 Javier Mendez Martinez - A01703446
*		 Jorge Lopez Chavez - A01209043
*
* Referencias:
*----------------------------------------*/
#include "steg.h"
using namespace std;
int main(int argc, char *argv[]) {
	FILE *imgFile, *msgFile, *imgFile2;
	int modo_ejecucion, count=0;
	char tempFileImg, temp, msg_temp;
// Comprobar que estamos dando los parametros correectos para ejecutar el programa
	if(argc != 5)
	{
        cout<<"Error, no hay parametros necesarios, [-a]: Agregar texto a la imagen  [-o]:Obtener texto en imagen <imagen fuente> <imagen destino> <mensaje.txt>";
		exit(1);
	}
// Revisar el modo_ejecucion de ejecucion del sistema, a para agregar texto, o para obtener texto
	if(!strcmp(argv[1], "-a"))
	{
		modo_ejecucion = 1;
	}
	else if(!strcmp(argv[1], "-o"))
	{
		modo_ejecucion = 0;
	}
//abrimos el archivo de la imagen en modo_ejecucion lectura
	imgFile = fopen(argv[2], "r");
	if (imgFile == NULL)
	{
		// caso de que no se pueda leer el archivo mandar mensaje y terminar programa
		cout<<stderr, "No se puede abrir el archivo %s\n", argv[2];
		exit(1);
	}
//Abrir archivo de texto en modo_ejecucion escritura
	imgFile2 = fopen(argv[3], "w");
	if (imgFile2== NULL)
	{
		// caso de que no se pueda leer el archivo mandar mensaje y terminar programa
		cout << stderr, "No se pudo crear el archivo de salida %s\n", argv[3];
		exit(1);
	}

// apuntamos al inicio del Archivo de la imagen 1 para obtner el desplazamiento y pasarlo a la imagen 2
	int desplazarmiento = obtener_desplazamiento(imgFile);
	rewind(imgFile);
	for(int i = 0; i < desplazarmiento; i++)
	{
		tempFileImg = fgetc(imgFile);
		fputc(tempFileImg, imgFile2);
		count++;
	}

//abrimos el 4 argumento, texto a guardar en la imagen (-a abrir regresara true )
	if(modo_ejecucion)
	{
		msgFile = fopen(argv[4], "r");
		if (msgFile== NULL)
		{
			//si el archivo txt no se abre mandar mensaje de error
			cout<<stderr, "NO se pudo abrir el archivo de texto %s\n", argv[4];
			exit(1);
		}

		int msg_size=  obtener_size_mensaje(msgFile);

		fputc(msg_size,imgFile2);
		count++;
		do
		{
			int mensaje;
			if(!feof(msgFile))
			{
				msg_temp = fgetc(msgFile);
				//ciclo para recorrer cada bit en la imagen 1
				for(int i = 1;i <= 8;i++)
				{
					temp = fgetc(imgFile);
					count++;
					// obtnemeos el valor lsb, para asi convertirlo en 0 si da 0 o en 1 si da 1
					int lsbFile = temp & 1;
					mensaje = obtener_bit(msg_temp, i);
					if(lsbFile == mensaje) {
						fputc(temp, imgFile2); // guardamos el cambio del bit en la imagen 2
					}
					else
					{
						if(lsbFile == 0)
						{
							temp = (temp | 1);
						}
						else
						{
							temp = (temp & ~1);
						}
						fputc(temp, imgFile2);
					}
				}
			}
			else
			{
				tempFileImg=fgetc(imgFile);
				fputc(tempFileImg,imgFile2);
				count++;
			}
		}while(!feof(imgFile));
		fclose(msgFile);
	}
	//En caso de estar en modo_ejecucion -o obtener mensaje se ejecuta lo siguiente
	else
	{
		msgFile=fopen(argv[4],"w");
		if (msgFile== NULL)
		{
			// abrir el archivo de texto y si nos e puede abrir mandar mensaje de error
			cout<< stderr, "Can't open text input file %s\n",argv[4];
			exit(1);
		}
		//Obtenemos el tamaño del mensaje en la imagen
		int msg_lenght = fgetc(imgFile);
		//recorremos el tamaño del mensaje
		for(int i=0;i<msg_lenght;i++)
		{
			char temp_ch = '\0';
			for(int j = 0; j < 8; j++) {
				temp_ch = temp_ch << 1;
				temp = fgetc(imgFile);
				int lsbFile = temp & 1;
				temp_ch |= lsbFile;
			}
			//guardamos el mensaje en el archivo de texto
			fputc(temp_ch, msgFile);
		}
		//cerramos los archivos
		fclose(msgFile);
	}
	//cerramos los archivos
	fclose(imgFile);
	fclose(imgFile2);
}
