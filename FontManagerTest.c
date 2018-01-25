#include <stdio.h>
#include <string.h>

#include "FontManager.h"
/***************************************************************************************/

int main( int     argc, char**  argv )
{

	FT_Error      error;

	if ( argc != 2 )
	{
		fprintf ( stderr, "usage: %s text\n", argv[0] );
		exit( 1 );
	}


	error = FontManager_Init(80, 50, 1, NULL,FontManager_ShowCallback_Sample);
	if ( error != 0 )
	{
		fprintf ( stderr, "Error FontManager_Init %d \n", error );
		exit( 1 );
	}

	FontManager_DrawString(argv[1], 0 , 0);
	FontManager_DrawString(argv[1], 25 , 25);


	FontManager_Show();

	FontManager_DeInit();

	return 0;
}

