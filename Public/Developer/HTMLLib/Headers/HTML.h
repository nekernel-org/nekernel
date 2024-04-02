/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>
#include <Headers/Window.h>

struct _HtmlBody;
struct _HtmlDOMElement;

CA_EXTERN_C VoidType HtmlLoadFile(const char* filePath, WindowPort* port,
                                  BooleanType enforceJavascript,
                                  BooleanType enforceHTTPS);

CA_EXTERN_C VoidType HtmlLoadURL(const char* filePath, WindowPort* port,
                                 BooleanType enforceJavascript,
                                 BooleanType enforceHTTPS);

CA_EXTERN_C VoidType HtmlLoadJavaScript(const char* code, WindowPort* port);

CA_EXTERN_C VoidType HtmlLoadTypeScript(const char* code, WindowPort* port);

typedef PtrVoidType HtmlPlugin;

CA_EXTERN_C HtmlPlugin HtmlLoadPlugin(const char* pluginPath, WindowPort* port);

CA_EXTERN_C VoidType HtmlUnloadPlugin(HtmlPlugin pluginHandle);

typedef struct _HtmlBody {
    CharacterTypeUTF8* Body;
    SizeType BodyLength;
} HtmlBody, *HtmlBodyRef;

/// @brief HTML Document, part of the DOM.
typedef struct _HtmlDOMElement {
    StrType255        fNameElement;    
    struct _HtmlBody* fBodyElement;

    struct _HtmlDOMElement* fNextElement;
    struct _HtmlDOMElement* fPrevElement;
    struct _HtmlDOMElement* fChildElement;
    struct _HtmlDOMElement* fParentElement;
} HtmlDOMElement, *HtmlDOMElementRef;


#define kHtmlRootElement "document"
