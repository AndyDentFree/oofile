/**
@file doxyExpatpp.h
Pure documentation header for Doxygen.

Documents items from the external expatpp framework.
see http://www.expatpp.com which maps to
http://sourceforge.net/projects/expatpp/
*/


/**
	\class expatppNesting
	Public domain expat wrapper with easy sub parser definition.
	Written for the OOFILE Report Writer to make it much easier
	to break up the parsing logic. Automatically returns to calling
	parser when tag completes.
	\par
	See OOF_XMLrepParser::startElement and the virtual methods it
	calls such as OOF_XMLrepParser::startElem_report for how simple
	it is to make a new parser active.
	\ingroup oofRepRead
*/
