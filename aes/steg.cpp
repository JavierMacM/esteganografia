#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include "encode.h"
#include "decode.h"
int get_image_data_offset(FILE* bmp_offset) {
	fseek(bmp_offset,10,0);
	int offset;
	offset=(int)fgetc(bmp_offset);
	return offset;
}

int get_message_length(FILE *fp) {
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return(size);
}
int get_bit(char the_byte,int which_bit) {
	return((the_byte>>8-which_bit)&1);
}

int main(int argc,char** argv) {
	unsigned char mask_table[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

	FILE *file_handle;
	FILE *message_handle;
	FILE *hidden_message_handle;

	if(argc!=5) {
		printf("*** Steganography by LSB substitution***\nUsage: %s [-e][-d] <source file> <destination file> <text file>\n-e : Add text to image\n -d : Get text from image\n",argv[0]);
		exit(1);
	}

	int mode;
	if(!strcmp(argv[1],"-e"))
		mode=1;
	else if(!strcmp(argv[1],"-d"))
		mode=0;
	else {
		printf("*** Steganography by LSB substitution***\nUsage: %s <mode> <source file> <destination file> <text file>\nMode - e = encrypt \n d= decrypt \n",argv[0]);
		exit(1);
	}

	/* HANDLING FILE OPENING AND ERRORS */
	file_handle=fopen(argv[2],"r");
	if (file_handle == NULL) {
		fprintf(stderr, "Can't open input file %s\n",argv[2]);
		exit(1);
	}

	hidden_message_handle=fopen(argv[3],"w");
	if (hidden_message_handle== NULL) {
		fprintf(stderr, "Cannot create output file %s\n",argv[3]);
		exit(1);
	}
	

	int c=0;

	/* Generate file with the same header. Copy first 128 bytes */
	char tmp_sig_cpy;
	int offset=get_image_data_offset(file_handle);

	rewind(file_handle);

	for(int i=0;i<offset;i++) {
		tmp_sig_cpy=fgetc(file_handle);
		fputc(tmp_sig_cpy,hidden_message_handle);
		c++;
	}
	/* Made file as .bmp */

	char file_buffer; 			// Temp variable for one byte from file
	char message_buffer;		// Temp buffer for one byte of message

	if(mode) {
        // Mensaje a encriptar.
        cout<<"Mensaje a cifrar: ";
        char mensaje[1024];
        cin.getline(mensaje, sizeof(mensaje));
        cout<<endl;
        // Para esta implementación, estaremos utilizando una llave de 16 bytes.
        unsigned char key[16];
        cout<<"Clave privada a 16 dígitos: ";
        for (int i = 0; i < 16; i++)
        {
            scanf("%hhu", &key[i]);
        }
        int originalLength = strlen((const char*)mensaje);
        int lengthPMessage = originalLength;
        // Revisar si el mensaje cabe en 16 bytes
        if (lengthPMessage % 16 != 0)
        {
            lengthPMessage = (lengthPMessage / 16 + 1) * 16;
        }
        unsigned char * pMessage = new unsigned char[lengthPMessage];
        for (int i = 0; i < lengthPMessage; i++)
        {
            if (i >= originalLength)
            {
                pMessage[i] = 0;
            }
            else
            {
                pMessage[i] = mensaje[i];
            }
        }
        unsigned char * encryptedMessage = new unsigned char[lengthPMessage];
        // Encriptar mensaje
        for (int i = 0; i < lengthPMessage; i+=16)
        {
            aes_encrypt(pMessage+i, key);
        }
        // Imprimir mensaje encriptado
        cout<<endl;
        cout <<"Mensaje cifrado en hexadecimal: "<<endl;
        for (int i = 0; i < lengthPMessage; i++)
        {
            printHexadecimal(pMessage[i], i, encryptedMessage);
            cout<<" ";
        }
        cout<<endl<<endl;
        // Write file with encrypted message
        ofstream outfile;
        outfile.open("topsecret.aes", ios::out | ios::binary);
        if (outfile.is_open())
        {
            outfile<<pMessage;
            outfile.close();
            cout<<"Operación exitosa: Archivo topsecret.aes creado"<<endl;
        }
        else
        {
            cout<<"Operación fallida: El archivo topsecret.aes no fue creado"<<endl;
        }
        delete[] pMessage;
        delete[] encryptedMessage;
		message_handle=fopen(argv[4],"r");
		if (message_handle== NULL) {
			fprintf(stderr, "Can't open text input file %s\n",argv[4]);
			exit(1);
		}
		int hidden_message_length=get_message_length(message_handle);


	/* 
	After offset has been read and the file header has been written as is for the virgin image - the length of the hidden message is written as the first byte. This length is then used while decrypting the text from the image. 
	*/
		fputc(hidden_message_length,hidden_message_handle);
		c++;
		do {
			int bit_of_message;
			if(!feof(message_handle)) {		
				message_buffer=fgetc(message_handle);
				for(int i=1;i<=8;i++) {  //Do this for every bit in every byte of the virgin-image

					file_buffer=fgetc(file_handle);
					c++;
					int file_byte_lsb = file_buffer & 1; // AND WITH 1 TO GET THE VALUE OF LSB. AND MAKES IT 0 IF LSB IS 0 OR 1 IF IT IS 1

					bit_of_message=get_bit(message_buffer,i);
					//bit_of_message=mask_table[i] & message_buffer;
					if(file_byte_lsb==bit_of_message) {
						fputc(file_buffer,hidden_message_handle);
					}
					else {
						if(file_byte_lsb==0)
							file_buffer = (file_buffer | 1);
						else
							file_buffer = (file_buffer & ~1);
						//  logic to flip the LSB bit of file_buffer and put it into a file with putc()
						fputc(file_buffer,hidden_message_handle);
					}
				}
			}
			else {
				tmp_sig_cpy=fgetc(file_handle);
				fputc(tmp_sig_cpy,hidden_message_handle);
				c++;
			}
		} while(!feof(file_handle));	
		fclose(message_handle);	
	}
	else {
		message_handle=fopen(argv[4],"w");
		if (message_handle== NULL) {
			fprintf(stderr, "Can't open text input file %s\n",argv[4]);
			exit(1);
		}
	
		/* Grab LSB of all bytes for length specified at fgetc */
		int message_length=fgetc(file_handle);
		for(int i=0;i<message_length;i++) {
			char temp_ch='\0';
			for( int j=0;j<8;j++) {
				temp_ch=temp_ch<<1;
				file_buffer=fgetc(file_handle);
				int file_byte_lsb = file_buffer & 1; 
				temp_ch|=file_byte_lsb;
			}
			fputc(temp_ch,message_handle);
		}
		fclose(message_handle);	
        string mensaje; // Variable para almacenar el mensaje
        ifstream topsecret; // Archivo de entrada
        topsecret.open(argv[4], ios::in | ios::binary); // Leer el archivo
        if (topsecret.is_open())
        {
            getline(topsecret, mensaje);
            cout<<"Leyendo archivo..."<<endl;
            topsecret.close();
        }
        else
        {
            fprintf(stderr, "%s: fallo al abrir el archivo del mensaje\n", argv[0]); // Error de archivo de entrada
        }
        char * msg = new char[mensaje.size()+1];
        strcpy(msg, mensaje.c_str()); // Copiar el mensaje a msg
        int messageLength = strlen((const char*)msg);
        unsigned char * encryptedMessage = new unsigned char[messageLength];
        for (int i = 0; i < messageLength; i++) {
            encryptedMessage[i] = (unsigned char)msg[i];
        }
        // Liberar memoria
        delete[] msg;
        unsigned char key[16];
        cout<<"Clave privada a 16 dígitos: ";
        for (int i = 0; i < 16; i++)
        {
            scanf("%hhu", &key[i]);
        }
        unsigned char expandedKey[176];
        keyExpansion(key, expandedKey);
        int messageLen = strlen((const char *)encryptedMessage);
        unsigned char * decryptedMessage = new unsigned char[messageLen];
        for (int i = 0; i < messageLen; i += 16) {
            aes_decrypt(encryptedMessage + i, expandedKey, decryptedMessage + i);
        }
        cout << "Mensaje descifrado en hexadecimal: "<< endl;
        for (int i = 0; i < messageLen; i++) {
            if (decryptedMessage[i] != '\0')
            {
                cout <<hex<< (int)decryptedMessage[i];
                cout << " ";
            }
            else
            {
                break;
            }
        }
        cout<<endl<<endl;
        cout<<"Mensaje descifrado: ";
        for (int i = 0; i < messageLen; i++) {
            if (decryptedMessage[i] != '\0')
            {
                cout<<decryptedMessage[i];
            }
            else
            {
                break;
            }
        }
        cout<<endl;
	}
	/* Clean up before exit */
	fclose(file_handle);
	fclose(hidden_message_handle);
}
