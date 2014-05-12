/*
Copyright (C) 2014 by Joachim Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once
#include <Polycode.h>

#define __NO_STD_VECTOR
#define __CL_ENABLE_EXCEPTIONS

#ifdef _WINDOWS
#include "CL/cl.hpp"
#elif defined(__APPLE__) && defined(__MACH__)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif //_WINODWS

namespace Polycode {

	template <typename OpType, long length>
	class _PolyExport PolyCL : public PolyBase {
	public:

		PolyCL(String kernelSource, String kernelName, std::vector< void* > arrays, std::vector< size_t > size, OpType *result) {
			try {
				init();
				buildProgram(kernelSource);
				createKernel(kernelName);
				createBuffers(arrays, size);
				executeKernel(result);
				cleanBuffers();
				success = true;
			}
			catch (cl::Error& err) {
				Logger::log("Error: %s\n", err.what());
				Logger::log("Error Code: %d\n", err.err());
				Logger::log("Build log: %d\n", programCL.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDeviceCL));
				success = false;
			}
		}

		PolyCL() {
			init();
		}

		void init() {
			result = new OpType[length];
			if (!setPlatforms()) {
				Logger::log("OpenCL disabled!\n");
				exit(EXIT_FAILURE);
			}
			else {
				setDevicesFromPlatform(platformsCL[0]);
				createContext();
			}
		}

		bool setPlatforms() {
			cl::Platform::get(&platformsCL);
			if (platformsCL.size() == 0) {
				Logger::log("OpenCL Platform fail\n");
				return false;
			}
			else {
				return true;
			}
		}

		bool setDevicesFromPlatform(const cl::Platform& platform) {

			// Extract a vector of devices
			platform.getDevices(CL_DEVICE_TYPE_GPU, &devicesCL);
			if (devicesCL.size() == 0) {
				platform.getDevices(CL_DEVICE_TYPE_CPU, &devicesCL);
				if (devicesCL.size() == 0) {
					Logger::log("No OpenCL Device\n");
					return false;
				}
				Logger::log("OpenCL Device Type: CPU\n");
			}
			else {
				Logger::log("OpenCL Device Type: GPU\n");
			}
			defaultDeviceCL = devicesCL[0];
			defaultDeviceCL.getInfo(CL_DEVICE_NAME, &deviceName);
			defaultDeviceCL.getInfo(CL_DEVICE_VENDOR, &vendorName);

			Logger::log("OpenCL Device found: %s %s\n", vendorName.c_str(), deviceName.c_str());
			
			return true;
		}

		void createContext() {
			contextCL = cl::Context(devicesCL);
			queueCL = cl::CommandQueue(contextCL, defaultDeviceCL);
		}

		void buildProgram(String kernelSource) {
			sourcesCL = cl::Program::Sources(1, std::make_pair(kernelSource.c_str(), kernelSource.length()));
			programCL = cl::Program(contextCL, sourcesCL);

			programCL.build(devicesCL);
		}

		void createKernel(String kernelName) {
			kernelCL = cl::Kernel(programCL, kernelName.c_str());
		}

		void createBuffers(std::vector< void* > arrays, std::vector< size_t > size) {
			for (int i = 0; i < arrays.size(); i++) {
				buffer.push_back(cl::Buffer(contextCL, CL_MEM_READ_WRITE, size[i]));
				queueCL.enqueueWriteBuffer(buffer[i], CL_TRUE, 0, size[i], arrays[i]);
				kernelCL.setArg(i, buffer[i]);
			}
			result_buffer = cl::Buffer(contextCL, CL_MEM_READ_WRITE, sizeof(OpType)*length);
			kernelCL.setArg(arrays.size(), result_buffer);
			
			Logger::log("OpenCL Buffers: allocated and set\n");
		}

		void executeKernel(OpType* result) {
			queueCL.enqueueNDRangeKernel(kernelCL, cl::NullRange, cl::NDRange(length), cl::NullRange, NULL, &event);
			
			queueCL.finish();
			queueCL.enqueueReadBuffer(result_buffer, CL_TRUE, 0, sizeof(OpType)*length, (OpType*)result);
			this->result = result;

			Logger::log("Result available now\n");
		}

		void cleanBuffers() {
			buffer.~vector();
		}

		void setDefaultDevice(cl::Device defaultDeviceCL) {
			this->defaultDeviceCL = defaultDeviceCL;
		}

		cl::Device getDefaultDevice() {
			return defaultDeviceCL;
		}

		std::string getDefaultDeviceName() {
			return deviceName;
		}

		cl::vector< cl::Platform > getPlatforms() {
			return platformsCL;
		}

		OpType* getResult() {
			return result;
		}

		bool getSuccess() {
			return success;
		}

	protected:
		cl::Device defaultDeviceCL;
		cl::Platform platformCL;
		cl::Kernel kernelCL;
		cl::Program programCL;
		cl::Program::Sources sourcesCL;
		cl::CommandQueue queueCL;
		cl::Context contextCL;
		cl::vector<cl::Device> devicesCL;
		cl::vector<cl::Platform> platformsCL;

		cl::vector<cl::Buffer> buffer;
		cl::Event event;
		cl::Buffer result_buffer;

		bool success;
		std::string vendorName;
		std::string deviceName;

		OpType* result;
	};
}
