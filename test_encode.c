/*
Name: Sivasubramanian
Date: 06/11/2023
Description : Steganography( Program to encode secret file data to source image file by creating new image file with option '-e' and decode data from new image file to new text file by using option '-d'.
Sample Input : Source image and secret text file.
Sample Output : Copying the secret data to image file and decoding it and getting the secret data in another file. 
*/

//declaring required header files

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

//main function
int main(int argc, char *argv[])
{
	if( argc <=3 )
	{   
		printf("ERROR : Please the Command Line Arguments.\n");
		printf("./a.out: Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
		printf("./a.out: Decoding : ./a.out -d <.bmp file> [output file]\n");
		return 1;
	}
	EncodeInfo encInfo;            //structure variable declaration
    DecodeInfo decInfo;            //structure variable declaration 
	int res = check_operation_type(argv);
	//checking res == encode or not
    if( res == e_encode)                 
	{
		//function definition for read and validate encode args and checking success or not
		if(read_and_validate_encode_args(argv, &encInfo) == e_success)
		{
			printf("INFO : Done\n");
		    //function definition for do encoding and checking success or not
			if ( do_encoding(&encInfo) == e_success)
			{
				//printing encoding done successfully
				printf("INFO : ## Encoding Done Successfully\n");
			}
			else
			{
				//printing error message
				printf("ERROR : Encoding Not Done successfully\n");
				return 2;
			}
		}
		else
		{
			//printing error message
			printf("ERROR : Read and validate args are failure\n");
			return 1;
		}
	}
	//checking for decode or not
	else if( res == e_decode)
	{
		printf("INFO : ## Decoding Procedure Stated ##\n");
		//function definition for read and validate decode args and checking success or not
		if(read_and_validate_decode_args(argv,&decInfo) == e_success)
		{
		  //function definition for do decoding and checking success or not
		   if( do_decoding ( &decInfo ) == e_success )
		   {
			 printf("INFO : ## Decoding Done Successfully ##\n");
	       }
		   else
		   {
			   //printing error message
			   printf("ERROR : Decoding unsuccessfull\n");
			   return 2;
		   }
	    }
		else
		{
			//printing error message
			printf("ERROR : Read and validate args are failure\n");
			return 1;
		}
	}
	else
	{ 
		//printing error message with usage
		printf("ERROR : Unsupported option\n");
		printf("./a.out: Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
		printf("./a.out: Decoding : ./a.out -d <.bmp file> [output file]\n");
	}
	
    return 0;
}

//function defintion for check operation
OperationType check_operation_type(char *argv[])
{
	//checking for encode
	if(strcmp(argv[1],"-e")==0)
	{
		return e_encode;
	}
	//checking for decode
	else if(strcmp(argv[1],"-d")==0)
	{
		return e_decode;
	}
	else
		return e_unsupported;
}
