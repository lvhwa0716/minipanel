#include <stdio.h>
#include <string.h>

#include "FontManager.h"
#include "MicroPanelGui.h"

/***************************************************************************************/

int main( int     argc, char**  argv )
{

	FT_Error      error;

	if ( argc != 2 )
	{
		fprintf ( stderr, "usage: %s text\n", argv[0] );
		exit( 1 );
	}

#if (0)
	error = FontManager_Init(80, 50, 1, NULL,FontManager_ShowCallback_Sample);
	if ( error != 0 )
	{
		fprintf ( stderr, "Error FontManager_Init %d \n", error );
		exit( 1 );
	}

	FontManager_DrawString(argv[1], 0 , -8);
	//FontManager_DrawString(argv[1], 25 , 25);


	FontManager_Show();

	FontManager_DeInit();
#else
	mpGui_Init();
	mpGui_ClearAll();
	mpGui_SetColor(1);
	#if (0)
		mpGui_HLine(0, 63, 0);
		mpGui_HLine(0, 64, 31);
		mpGui_VLine(0, 0, 31);
		mpGui_VLine(63, 0, 31);
		mpGui_Line(0, 0, 63,31);
		mpGui_Line(0, 0, 63,16);
		mpGui_Line(0, 0, 32,31);
	#endif
	#if (0)
		mpGui_FillRect(0,0, 127,31);
	#endif

	#if (1)
		mpGui_Rect(0,0, 127,31);
		mpGui_Line(0,0, 64,31);
		mpGui_Line(64,31, 127,0);
	#endif
	//mpGui_VLine(1, 1, 24);

	//mpGui_FillRect(1, 1, 32, 31);
	FontManager_DrawString(argv[1], 1 , 1 , 1);

	FontManager_DrawString(argv[1], 1 , 16 , 0);

	mpGui_UpdateScreen(0,0,128,32);
	mpGui_DeInit();
#endif
	return 0;
}

