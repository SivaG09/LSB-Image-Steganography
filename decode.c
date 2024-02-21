/*
Name: Sivasubramanian
Date: 06/11/2023
Description: Decode Part of Steganography
*/

#include<stdio.h>
#include<string.h>
#include"types.h"
#include"common.h"
#include"decode.h"

/*Function Definitions*/

//Function definition of read and validate decode args
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{   
	//comparing bmp file or not
	if(strcmp(strstr(argv[2],".bmp"),".bmp") == 0)
	{  
		//if yes, printing validated and storing the file name to stego file name
		printf("INFO : BMP Validated Successfully\n");
		printf("INFO : Opened %s\n",argv[2]);
		decInfo->stego_image_fname = argv[2];
	}
	else
	{   
		//if not, printing not an BMP file
		printf("INFO : NOT an BMP file\n");
		return e_failure;
	}
	//checking 3rd CLA present or not
	if(argv[3]!= NULL)
	{
		decInfo->secret_data_fname = argv[3];
		printf("INFO : Opened %s\n",argv[3]);
	}
	else
	{
		decInfo->secret_data_fname = "decoded.txt";
		printf("INFO : Output file not mentioned. Creating decoded.txt as default");
		printf("Opened decoded.txt\n");
	}
	return e_success;
}

//function definition for opening decoding files
Status open_decoding_files(DecodeInfo *decInfo)
{  
	//opening stego image file in read mode
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"r");
	//Do error handling
	if(decInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr,"ERROR : Unable to open file %s\n",decInfo->stego_image_fname);
		return e_failure;
	}
	//opening secret_data file in write mode
	decInfo->fptr_secret_data = fopen(decInfo->secret_data_fname,"w");
	//Do error handling
	if(decInfo->fptr_secret_data == NULL)
	{
		perror("fopen");
		fprintf(stderr,"ERROR : Unable to open file %s\n",decInfo->secret_data_fname);
		return e_failure;
	}
	return e_success;
}

//function definition for decoding image to data
Status decode_image_to_data(DecodeInfo *decInfo)
{
	//for loop to get 8 bits of data
	for ( int j = 0; j < 8; j++ )
	{
		//doing bit operations to get the values
		decInfo->decode_data[0] |= ((decInfo->str[j] & 1) << j);
	}
	return e_success;
}

//function defintion for decode size to data
Status decode_size_to_data(DecodeInfo *decInfo)
{   
	//reading 32 bits from stego image file to secret file size
	fread(decInfo->secret_file_size,32,1,decInfo->fptr_stego_image);
	decInfo->image_data_size = 0;
	for( int i = 0; i < 32; i++)
	{
		decInfo->image_data_size |= ((decInfo->secret_file_size[i] & 1) << i);
	}
	return e_success;
}

// function definition for decode secret file extn size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{ 
	//calling function decode size to data
	decode_size_to_data(decInfo);
	//printing the file extension size
	printf("File extension size : %d\n",decInfo->image_data_size);
	return e_success;
}

//function definition for decode secret file extn
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	for( int i = 0; i < decInfo->image_data_size; i++ )
	{
		fread(decInfo->str, 8, 1, decInfo->fptr_stego_image);
	    decInfo->decode_data[0] = 0;
		decode_image_to_data(decInfo);
		printf("%c", decInfo->decode_data[0]);
	}
	printf("\nFile pointer is %ld\n",ftell(decInfo->fptr_stego_image));
	return e_success;
}

//function definition for decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	//calling function size to data
	decode_size_to_data(decInfo);
	printf("File size %d\n",decInfo->image_data_size);
	printf("File pointer is %ld\n",ftell(decInfo->fptr_stego_image));
	return e_success;
}

//function defintion for decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	for ( int i = 0; i< decInfo->image_data_size; i++)
	{   
		//reading from stego image
		fread(decInfo->str,8,1,decInfo->fptr_stego_image);
		decInfo->decode_data[0] = 0;
        //calling func decode image to data
		decode_image_to_data(decInfo);

		printf("%c",decInfo->decode_data[0]);
		fprintf(decInfo->fptr_secret_data,"%c",decInfo->decode_data[0]);
	}
	printf("\nFile pointer is %ld\n",ftell(decInfo->fptr_stego_image));
	return e_success;
}

//function defintion for decode magic string
Status decode_magic_string(DecodeInfo *decInfo)
{   
	//making offset to 54th byte
	fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
	//for loop to run till size of magic string
    for ( int i = 0; i < strlen(MAGIC_STRING); i++)
	{
	   	fread(decInfo->str, 8, 1, decInfo->fptr_stego_image);
     	decInfo->decode_data[0] = 0;
        //function calling of decode image to data 
	    decode_image_to_data(decInfo);
        //printing the magic string
		printf("%c",decInfo->decode_data[0]);
	  
		if(decInfo->decode_data[i] == MAGIC_STRING[i])
		  {
			  continue;
		  }
		  else
		  {
			return e_failure;
	      }
	    }
	printf("\n");
	return e_success;
}

//function defintion for do decoding
Status do_decoding(DecodeInfo *decInfo)
{   
	//function call for opening decoding files and checking it is success or not
	if(open_decoding_files(decInfo) == e_success)
	{
		printf("INFO : Files opened Successfully\n");
	    //function call for decode magic string and checking it is success or not
		if(decode_magic_string(decInfo) == e_success)
		{
			printf("INFO : Decoding Magic string signature\n");
			printf("INFO : Done\n");
 	        //function call for decode secret file extn size and checking it is success or not
			if(decode_secret_file_extn_size(decInfo) == e_success)
			{
				printf("INFO : Decoding secret file extn size\n");
				printf("INFO : Done\n");
	            //function call for decode secret file extn and checking it is success or not
				if(decode_secret_file_extn(decInfo) == e_success)
				{
					printf("INFO : Decoding secret file extn\n");
					printf("INFO : Done\n");
	                //function call for decode secret file size and checking it is success or not
					if(decode_secret_file_size(decInfo) == e_success)
					{
						printf("INFO : Decoding secret file size\n");
						printf("INFO : Done\n");
	                    //function call for decode secret file data and checking it is success or not
						if(decode_secret_file_data(decInfo) == e_success)
						{
							printf("INFO : Decoding secret file data\n");
							printf("INFO : Done\n");
						}
						else
						{  
							//printing error message and returning failure
							printf("ERROR : Decoding secret file size failed\n");
						    return e_failure;
						}
					}
					else
					{
						//printing error message and returning failure
						printf("ERROR : Decoding secret file size failed\n");
						return e_success;
					}
				}
				else
				{
					//printing error message and returning failure
					printf("ERROR : Decoding secret file extn failed\n");
					return e_failure;
				}
			}
			else
			{
				//printing error message and returning failure
				printf("ERROR : Decoding secret file extn size failed\n");
				return e_failure;
			}

		}
		else
		{
			//printing error message and returning failure
			printf("ERROR : Decoding magic string signature failed\n");
			return e_failure;
		}
	}
	else
	{
	    //printing error message and returning failure
		printf("ERROR : Failed to open files\n");
		return e_failure;
	}
	return e_success;
}

