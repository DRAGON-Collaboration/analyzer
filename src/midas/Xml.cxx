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

midas::Xml::Xml(const char* filename):
	fTree(0), fOdb(0)
{
	Init(filename);
}

midas::Xml::~Xml()
{
	if(fTree) mxml_free_tree(fTree);
}

void midas::Xml::Init(const char* filename)
{
	char err[256]; int err_line;
	fTree = ParseFile(filename, err, sizeof(err), &err_line);
	if(!fTree) {
		std::cerr << "Error: Bad XML file: " << filename << ", error message: " <<
			 err << ", error line: " << err_line << "\n";
		return;
	}
	fOdb = mxml_find_node(fTree, "/odb");
	if(!fOdb) {
		std::cerr << "Error: no odb tag found in xml file: " << filename << ".\n";
	}
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
	std::vector<std::string> nodes = path_tokenize(path);
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
