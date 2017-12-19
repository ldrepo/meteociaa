/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

#ifndef LCD_CUSTCHARS_H_
#define LCD_CUSTCHARS_H_

/*==================[inclusions]=============================================*/

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

// Macros para definir caracteres personalizado	
#define X )*2+1
#define _ )*2
#define s (((((0 /* For building glyphs 5 bits wide */
    
/*==================[typedef]================================================*/
	
/*==================[external data declaration]==============================*/

// Caracter personalizado carita feliz :)
const char lcdSymbol0[8] = {
	s     _ X X X _     ,
	s     _ X _ X _     ,
	s     _ X _ X _     ,
	s     _ X X X _     ,
	s     _ X X X _     ,
	s     X X X X X     ,
	s     X X X X X     ,
	s     _ X X X _
};

const char lcdSymbol1[8] = {
	s     _ _ X _ _     ,
	s     _ _ X _ _     ,
	s     _ X X X _     ,
	s     _ X X X _     ,
	s     X _ X X X     ,
	s     X _ X X X     ,
	s     X X X X X     ,
	s     _ X X X _
};


const char lcdSymbol2[8] = {
	s     _ _ _ _ _   ,
	s     _ _ X X _   ,
	s     _ X _ _ X   ,
	s     _ X _ _ X   ,
	s     _ _ X X _   ,
	s     X X _ X X   ,
	s     X _ _ _ _   ,
	s     _ _ _ _ _   ,
};


const char lcdSymbol3[8] = {
	s     _ X X X _     ,
	s     X X X X X     ,
	s     X X _ X X     ,
	s     X _ _ _ X     ,
	s     X _ _ _ X     ,
	s     X X _ X X     ,
	s     X X X X X     ,
	s     _ X X X _
};


const char lcdSymbol4[8] = {
s     X X _ _ _     ,
s     X X _ _ _     ,
s     _ X X X _     ,
s     X X _ X X     ,
s     X X _ _ _     ,
s     X X _ _ _    ,
s     X X _ X X     ,
s     _ X X X _
};

/*==================[external functions declaration]=========================*/

/*==================[cplusplus]==============================================*/

/*==================[end of file]============================================*/

#endif /* LCD_CUSTCHARS_H_ */




