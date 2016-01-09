// Deep Shah
// Shashank Seeram
// Modified tokenizer


#include "tokenizer.h"


//check to see if the current character is alphanumeric
int isAlphaNum(char c) { 
  int cNum;
  int cUpperAlpha;
  int cLowerAlpha;
  cNum = c - '0';
  cUpperAlpha = c - 'A';
  cLowerAlpha = c - 'a';
  if (cNum >= 0 && cNum <= 9) {
    return 1;
  }
  else if (cUpperAlpha >= 0 && cUpperAlpha <= 25) {
    return 1;
  }
  else if (cLowerAlpha >= 0 && cLowerAlpha <= 25) {
    return 1;
  }
  else {
    return 0;
  }
}

 // Initializes and reinitializes the input struct
void resetInput(Input *iPtr, FILE *fp) {
	iPtr->fp = fp;
	iPtr->used = 0;
	iPtr->length = 0;
}


 //readInput reads and return a single char from the input struct
 // It also returns the char as a lowercase letter if it is uppercase
int readInput(Input *iPtr) {
	char c;
	// Chars available to read.
	if (iPtr->used < iPtr->length)
	{
		c = iPtr->buffer[iPtr->used];
		iPtr->used++;
		return tolower(c);
	}
	// Need to read more chars from the file stream.
	else if ((iPtr->length = fread(iPtr->buffer, sizeof(char), sizeof(iPtr->buffer), iPtr->fp)) < 1)
	{
		return -1;
	}
	else
	{
		c = iPtr->buffer[0];
		iPtr->used = 1;
		return tolower(c);
	}
}


void IDestroy(Input *iPtr) {
  // Simply frees the tokenizer object it receives via a pointer.
  free(iPtr);
}

char *IGetNextToken(Input *iPtr) {
  char *temp;
  char *token;
  int maxlength;
  int length;
  char c;
  temp = (char *) malloc(2048 * sizeof(char));
  maxlength = 2048;
  length = 0;
  // Reading in any separator characters before the next token.
  while ((c = readInput(iPtr)) != -1 && isAlphaNum(c) == 0) {
	  
  }
  // Making sure the first char is included.
  if (isAlphaNum(c) == 1) {
	  if (length >= maxlength) {
		  temp = (char *) realloc(temp, (maxlength * 1.5) * sizeof(char));
		  maxlength = maxlength * 1.5;
	  }
	  temp[length] = c;
	  length++;
  }
  // Continuously looping to create the token. If it becomes too large the buffer is realloc'd.
  while ((c = readInput(iPtr)) != -1 && isAlphaNum(c) == 1) {
	  if (length >= maxlength) {
		  temp = (char *) realloc(temp, (maxlength * 1.5) * sizeof(char));
		  maxlength = maxlength * 1.5;
	  }
	  temp[length] = c;
	  length++;
  }
  // No token.
  if (length <= 0) {
	  free(temp);
	  return NULL;
  }
  else {
	  token = (char *) malloc((length + 1) * sizeof(char));
	  strncpy(token, temp, length);
	  token[length] = '\0';
	  free(temp);
	  return token;
  }
}
