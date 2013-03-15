//! \file Xml.cxx
//! \author G. Christian
//! \brief Implements Xml.hxx
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "Xml.hxx"

#ifdef USE_ROOT
#include <TSystem.h>
#include <TString.h>
#endif

midas::Xml::Xml(const char* filename):
	fTree(0), fOdb(0), fIsZombie(false)
{
#ifdef USE_ROOT
	TString fnameExp (filename);
	gSystem->ExpandPathName(fnameExp);
	const char* fname2 = fnameExp.Data();
#else
	const char* fname2 = filename;
#endif

	char err[256]; int err_line;
	fTree = ParseFile(fname2, err, sizeof(err), &err_line);
	if(!fTree) {
		std::cerr << "Error: Bad XML file: " << fname2 << ", error message: " <<
			 err << ", error line: " << err_line << "\n";
		fIsZombie = true;
		return;
	}
	fOdb = mxml_find_node(fTree, "/odb");
	if(!fOdb) {
		std::cerr << "Error: no odb tag found in xml file: " << fname2 << ".\n";
		fIsZombie = true;
		return;
	}
}

midas::Xml::Xml(char* buf, int length):
	fTree(0), fOdb(0), fIsZombie(false)
{
	char err[256]; int err_line;
	fTree = ParseBuffer(buf, length, err, sizeof(err), &err_line);
	if(!fTree) {
		std::cerr << "Error: Bad XML bufer, error message: " <<
			 err << ", error line: " << err_line << "\n";
		fIsZombie = true;
		return;
	}
	fOdb = mxml_find_node(fTree, "/odb");
	if(!fOdb) {
		std::cerr << "Error: no odb tag found in xml buffer.\n";
		fIsZombie = true;
		return;
	}
}

midas::Xml::~Xml()
{
	if(fTree) mxml_free_tree(fTree);
}


midas::Xml::Node midas::Xml::ParseFile(const char* file_name, char *error, int error_size, int *error_line)
{
	char* buf, line[1000];
	int length = 0;
	PMXML_NODE root;
	FILE* f;

	if (error)
		 error[0] = 0;

	f = fopen(file_name, "r");

	if (!f) {
		sprintf(line, "Unable to open file \"%s\": ", file_name);
		strlcat(line, strerror(errno), sizeof(line));
		strlcpy(error, line, error_size);
		return NULL;
	}

	fpos_t startPos;
	int start_found(0);
	while(1) {
		if(!start_found) {
			char str[256];
			size_t nRead = fread(str, 1, strlen("<odb"), f);
			fseek(f, -1*(strlen("<odb")-1), SEEK_CUR);
			if(nRead != strlen("<odb")) {
				sprintf(error, "Could not find \"<odb\"");
				*error_line = __LINE__;
				fclose(f);
				return NULL;
			}
			if(!memcmp(str, "<odb", strlen("<odb"))) {
				fseek(f, -1, SEEK_CUR);
				fgetpos(f, &startPos);
				start_found = 1;
			}
		}
		else {
			char str[256];
			size_t nRead = fread(str, 1, strlen("</odb>"), f);
			fseek(f, -1*(strlen("</odb>")-1), SEEK_CUR);
			length++;
			if(nRead != strlen("</odb>")) {
				sprintf(error, "Could not find \"</odb>\"");
				*error_line = __LINE__;
				fclose(f);
				return NULL;
			}
			if(!memcmp(str, "</odb>", strlen("</odb>"))) {
				break;
			}
		}
	}

	length += strlen("</odb>");
	fsetpos(f, &startPos);
	buf = (char *)malloc(length+1);
	if (buf == NULL) {
		fclose(f);
		sprintf(line, "Cannot allocate buffer: ");
		strlcat(line, strerror(errno), sizeof(line));
		strlcpy(error, line, error_size);
		return NULL;
	}

  /* read odb portion of the file */
	length = (int)fread(buf, 1, length, f);
	buf[length] = 0;
	fclose(f);

	if (mxml_parse_entity(&buf, file_name, error, error_size, error_line) != 0) {
		free(buf);
		return NULL;
	}

	root = mxml_parse_buffer(buf, error, error_size, error_line);

	free(buf);

	return root;

}

midas::Xml::Node midas::Xml::ParseBuffer(char* buf, int length, char *error, int error_size, int *error_line)
{
	PMXML_NODE root;

	if (error)
		 error[0] = 0;

	int startPos = 0, lodb = (int)strlen("<odb");
	while(startPos < length + lodb) {
		if(!memcmp(&buf[startPos], "<odb", strlen("<odb")))
			break;
		++startPos;
	}
	if(startPos == length + lodb) {
		sprintf(error, "Could not find \"<odb\"");
		*error_line = __LINE__;
		return NULL;
	}

	char* pxml = &buf[startPos];
	if (mxml_parse_entity(&pxml, "", error, error_size, error_line) != 0) {
		return NULL;
	}

	root = mxml_parse_buffer(pxml, error, error_size, error_line);

	return root;
}


bool midas::Xml::Check()
{
	if(fTree && fOdb) return true;
	std::cerr << "Warning: midas::Xml object was initialized with a bad XML file, "
						<<"cannot perform any further operations.\n";
	return false;
}

namespace {
inline std::vector<std::string> path_tokenize(const char* path) {
	std::string strPath(path);
	std::vector<std::string> nodes;
	long posn (0);
	while(strPath.find("/") < strPath.size()) {
		posn = strPath.find("/");
		nodes.push_back(strPath.substr(0, posn));
		strPath = strPath.substr(posn+1);
	}
	nodes.push_back(strPath);
	return nodes;
}
std::string get_xml_path(const char* path, const char* node_type) {
	const char* pPath = path[0] == '/' ? &path[1] : &path[0];
	std::vector<std::string> nodes = path_tokenize(pPath);
	std::stringstream xmlPath;
	for(std::vector<std::string>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if(it != nodes.end() - 1) xmlPath << "/dir[@name=" << *it << "]";
		else xmlPath << "/" << node_type << "[@name=" << *it << "]";
	}
	return xmlPath.str();
} }

midas::Xml::Node midas::Xml::FindKey(const char* path)
{
	if(!Check()) return 0;
	Node out = mxml_find_node(fOdb, get_xml_path(path, "key").c_str());
	if(!out) {
		std::cerr << "Error: XML path: " << path << " was not found.\n";
	}
	return out;
}

midas::Xml::Node midas::Xml::FindKeyArray(const char* path)
{
	if(!Check()) return 0;
	Node out = mxml_find_node(fOdb, get_xml_path(path, "keyarray").c_str());
	if(!out) {
		std::cerr << "Error: XML path: " << path << " was not found.\n";
	}
	return out;
}



#ifdef MIDAS_XML_TESTING
int main() {
	midas::Xml mxml("run23822.mid");
	midas::Xml::Node loc = mxml.FindKey("System/Clients/19764/Host");
	if(!loc) return 1;
	
	int i = -1;
	std::vector<short> peds;
	mxml.GetArray("Equipment/gTrigger/Variables/Pedestals", peds);
	for(int i=0; i< peds.size(); ++i) {
		std::cout << "peds[" << i << "]: " << peds[i] << "\n";
	}
}
#endif
