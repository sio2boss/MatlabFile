/*
 * MatlabFile.hpp
 *
 *  Created on: Jan 25, 2013
 *      Author: sio2
 */

#ifndef MATLABFILE_HPP_
#define MATLABFILE_HPP_

#include <string>
#include <hdf5.h>
#include <vector_types.h>

enum MatlabFileMode {READ, WRITE};

/**
 * @class
 * @brief wrapper for HDF5 file format with Matlab header
 */
class MatlabFile {
public:
	MatlabFile(const MatlabFileMode& mode);
	virtual ~MatlabFile();

	long open(const std::string &filename);

	long writeArray(const std::string &variable_name, Float2dArray* array);
	long readArray(const std::string &variable_name, Float2dArray* &array);

	void close();

protected:

	hid_t file;
	hid_t create_plist;
	herr_t status;

	MatlabFileMode mode;
	std::string filename;

	static const int HEADER_SIZE = 512;

};

#endif /* MATLABFILE_HPP_ */
