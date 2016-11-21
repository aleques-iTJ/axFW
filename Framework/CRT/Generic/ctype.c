#include <ctype.h>


int isalnum(int character)
{
	return	(character >= 'a' && character <= 'z') || 
			(character >= 'A' && character <= 'Z') || 
			(character >= '0' && character <= '9');
}

int isalpha(int character)
{
	return	(character >= 'a' && character <= 'z') ||
			(character >= 'A' && character <= 'Z');
}

int iscntrl(int character)
{
	return	(character >= 0x00 && character <= 0x1F) ||
			(character == 0x7F);
}

int isdigit(int character)
{
	return (character >= '0' && character <= '9');
}

int isxdigit(int character)
{
	return  (character >= '0' && character <= '9') ||
			(character >= 'a' && character <= 'f') ||
			(character >= 'A' && character <= 'F');			
}

int islower(int character)
{
	return (character >= 'a' && character <= 'z');
}

int isupper(int character)
{
	return (character >= 'A' && character <= 'Z');
}

int isspace(int character)
{
	return  (character == ' ' ) ||
			(character == '\t') ||
			(character == '\n') ||
			(character == '\v') ||
			(character == '\f') ||
			(character == '\r');
}