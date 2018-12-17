#ifndef VEDA_CONTEXT_H
#define VEDA_CONTEXT_H

#include <configParser.h>
#include <SFML/Audio.h>
#include <setjmp.h>
#include "sprite.h"
#include "typedef.h"

#define ERROR_BUFFER_SIZE 4096

typedef struct ContextStruct Context;

bool	getInteger(ParserObj *obj, void *data, char *err_buffer);
bool	getUintVector(ParserObj *obj, void *data, char *err_buffer);
bool	getFloatVector(ParserObj *obj, void *data, char *err_buffer);
bool	gefUFloatVector(ParserObj *obj, void *data, char *err_buffer);
bool	getDuppedString(ParserObj *obj, void *data, char *err_buffer);
bool	getFloatingNumber(ParserObj *obj, void *data, char *err_buffer);
bool	getPositiveInteger(ParserObj *obj, void *data, char *err_buffer);
bool	copyStringInBuffer(ParserObj *obj, void *data, char *err_buffer);
bool	getPositiveFloatingNumber(ParserObj *obj, void *data, char *err_buffer);

bool	getObjectElement(Context *context);

typedef enum contextTypes {
	ContextBooleanType	= 1 << 0,
	ContextStringType	= 1 << 1,
	ContextFloatType	= 1 << 2,
	ContextArrayType	= 1 << 3,
	ContextObjType		= 1 << 4,
	ContextListType		= 1 << 5,
	ContextIntType		= 1 << 6,
	ContextNullType		= 1 << 7,
} contextTypes;

typedef enum contextActions {
	DO_NOTHING,
	DISPLAY_WARNING,
	LONG_JUMP,
	CALLBACK,
} contextActions;

typedef enum contextIssue {
	MISSING_PARAMETER = 1,
	INVALID_TYPE,
	USAGE_FAILED,
} contextIssue;

typedef struct SpriteLoaded {
	Sprite	sprite;
	bool	needToDestroy;
} SpriteLoaded;

typedef struct MusicLoaded {
	sfMusic	*music;
	bool	needToDestroy;
} MusicLoaded;

struct ContextStruct {
	ParserObj	*object;
	jmp_buf		jumpBuffer;
	char		error[ERROR_BUFFER_SIZE];
	int		expectedType;
	void		*data;
	char		*index;
	contextActions	onMissing;
	contextActions	onInvalidType;
	contextActions	onUseFail;
	void		(*callback)(Context *context, contextIssue issue);
	bool		(*useElement)(ParserObj *obj, void *data, char *err_buffer);
};

#endif //VEDA_CONTEXT_H
