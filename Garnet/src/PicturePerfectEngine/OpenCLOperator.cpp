
#include <CL/cl.h>

#include <PicturePerfectEngine/OpenCLOperator.h>

//-------------------------------------------------------------------
//-------------------------------------------------------------------

class OpenCLObject
{
public :
	OpenCLObject();
	~OpenCLObject();

	cl_context getContext() { return _context; }
	cl_device_id getDeviceId() { return _device_id; }
	cl_command_queue getCommandQueue() { return _command_queue; }

private :
	cl_context _context;
	cl_platform_id _platform_id;
	cl_device_id _device_id;
	cl_command_queue _command_queue;
};

//-------------------------------------------------------------------
//-------------------------------------------------------------------

OpenCLObject::OpenCLObject()
{
	cl_platform_id platform_id[1] = { NULL };
	cl_device_id device_id[1] = { NULL };
	cl_uint num_platforms;
	cl_uint num_devices;
	cl_int ret;
	char buf[128];
	size_t size_ret;

	ret = clGetPlatformIDs(1, platform_id, &num_platforms);
	clGetPlatformInfo(platform_id[0], CL_PLATFORM_PROFILE, sizeof(buf), buf, &size_ret);
	std::cout << "CL_PLATFORM_PROFILE: " << buf << std::endl;
	clGetPlatformInfo(platform_id[0], CL_PLATFORM_VERSION, sizeof(buf), buf, &size_ret);
	std::cout << "CL_PLATFORM_VERSION: " << buf << std::endl;
	clGetPlatformInfo(platform_id[0], CL_PLATFORM_NAME, sizeof(buf), buf, &size_ret);
	std::cout << "CL_PLATFORM_NAME: " << buf << std::endl;
	clGetPlatformInfo(platform_id[0], CL_PLATFORM_VENDOR, sizeof(buf), buf, &size_ret);
	std::cout << "CL_PLATFORM_VENDOR: " << buf << std::endl;
	clGetPlatformInfo(platform_id[0], CL_PLATFORM_EXTENSIONS, sizeof(buf), buf, &size_ret);
	std::cout << "CL_PLATFORM_EXTENSIONS: " << buf << std::endl;

	ret = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_DEFAULT, 1, device_id, &num_devices);
	clGetDeviceInfo(device_id[0], CL_DEVICE_NAME, sizeof(buf), buf, &size_ret);
	std::cout << "CL_DEVICE_NAME: " << buf << std::endl;
	clGetDeviceInfo(device_id[0], CL_DEVICE_VENDOR, sizeof(buf), buf, &size_ret);
	std::cout << "CL_DEVICE_VENDOR: " << buf << std::endl;
	clGetDeviceInfo(device_id[0], CL_DRIVER_VERSION, sizeof(buf), buf, &size_ret);
	std::cout << "CL_DRIVER_VERSION: " << buf << std::endl;
	clGetDeviceInfo(device_id[0], CL_DEVICE_PROFILE, sizeof(buf), buf, &size_ret);
	std::cout << "CL_DEVICE_PROFILE: " << buf << std::endl;
	clGetDeviceInfo(device_id[0], CL_DEVICE_OPENCL_C_VERSION, sizeof(buf), buf, &size_ret);
	std::cout << "CL_DEVICE_OPENCL_C_VERSION: " << buf << std::endl;

	_platform_id = platform_id[0];
	_device_id = device_id[0];
	_context = clCreateContext(NULL, 1, device_id, NULL, NULL, &ret);
	_command_queue = clCreateCommandQueue(_context, device_id[0], 0, &ret);
}

OpenCLObject::~OpenCLObject()
{
	clReleaseCommandQueue(_command_queue);
	clReleaseContext(_context);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

static OpenCLObject*  _clInstance = 0;

OpenCLObject*  getOpenCLObject()
{
	return _clInstance;
}

void OpenCLOperator::initialize()
{
	if ( !_clInstance ) {
		_clInstance = new OpenCLObject();
	}
}

void OpenCLOperator::finalize()
{
	if ( _clInstance ) {
		delete _clInstance;
	}
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
const char _sourceAddI1Bg[] = 
	"__kernel void AddI1Bg(__global uchar* src1, __global uchar* src2, __global uchar* dest)"
	"{"
	"    int gid = get_global_id(0);"
	"    int temp = src1[gid] + src2[gid];"
	"    dest[gid] = (uchar)min(temp, 255);"
	"}";
const char _kernelName[] = "AddI1Bg";

namespace OpenCLOperator
{
class Add : public Operator
{
private :
	cl_mem _memSrc1;
	cl_mem _memSrc2;
	cl_mem _memDest;
	cl_program _program;
	cl_kernel _kernel;
	bool _isPrepared;
public :
	Add()
	{
		_isPrepared = false;
	}
	~Add()
	{
		cleanup();
	}

	void setup(uint32_t width, uint32_t height) {
		if ( !_isPrepared  ) {
			cl_context context = getOpenCLObject()->getContext();
			cl_device_id device_id = getOpenCLObject()->getDeviceId();
			cl_int ret;

			_memSrc1 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(uint8_t) * width * height, NULL, &ret);
			_memSrc2 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(uint8_t) * width * height, NULL, &ret);
			_memDest = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(uint8_t) * width * height, NULL, &ret);

			const char* source = _sourceAddI1Bg;
			size_t sourceLen = sizeof(_sourceAddI1Bg);
			_program = clCreateProgramWithSource(context, 1, (const char**)&source, (const size_t*)&sourceLen, &ret);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			ret = clBuildProgram(_program, 1, &device_id, NULL, NULL, NULL);
			{//if ( ret != CL_SUCCESS ) { 
				//std::cerr << __LINE__ << ":" << ret << std::endl;
				cl_int rret;
				char buf[4096];
				size_t size_ret;
				rret = clGetProgramBuildInfo(_program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buf), buf, &size_ret);
				if ( rret == CL_SUCCESS ) {
					std::cerr << buf << std::endl;
				}
			}
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			_kernel = clCreateKernel(_program, _kernelName, &ret);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }

			_isPrepared = true;
		}
	}

	void  cleanup()
	{
		cl_int ret;
		ret = clReleaseKernel(_kernel);
		ret = clReleaseProgram(_program);
		ret = clReleaseMemObject(_memSrc1);
		ret = clReleaseMemObject(_memSrc2);
		ret = clReleaseMemObject(_memDest);
	}

	virtual ResultType  process(const strings& tokens, PacketStore& packets)
	{
		assertNumberOfArgs(tokens, 4);

		// 
		Packet& src1 = packets[tokens[2]];
		Packet& src2 = packets[tokens[3]];
		Packet& dst  = packets[tokens[1]];

		//
		uint32_t width, height;
		width = src1.image->width;
		height = src1.image->height;
		setup(width, height);

		// Image
		if ( _isPrepared ) {
			cl_command_queue command_queue = getOpenCLObject()->getCommandQueue();
			cl_int ret;
			ret = clEnqueueWriteBuffer(command_queue, _memSrc1, CL_TRUE, 0, sizeof(uint8_t) * width * height, src1.image->imageData, 0, NULL, NULL);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			ret = clEnqueueWriteBuffer(command_queue, _memSrc2, CL_TRUE, 0, sizeof(uint8_t) * width * height, src2.image->imageData, 0, NULL, NULL);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			ret = clSetKernelArg(_kernel, 0, sizeof(cl_mem), (void*)&_memSrc1);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			ret = clSetKernelArg(_kernel, 1, sizeof(cl_mem), (void*)&_memSrc2);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			ret = clSetKernelArg(_kernel, 2, sizeof(cl_mem), (void*)&_memDest);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			size_t global_work_size = width * height;
			size_t local_work_size = 1;
			ret = clEnqueueNDRangeKernel(command_queue, _kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			ret = clEnqueueReadBuffer(command_queue, _memDest, CL_TRUE, 0, sizeof(uint8_t) * width * height, dst.image->imageData, 0, NULL, NULL);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			ret = clFlush(command_queue);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
			ret = clFinish(command_queue);
			if ( ret != CL_SUCCESS ) { std::cerr << __LINE__ << ":" << ret << std::endl; abort(); }
		}
#if 0
		cvAdd(src1.image, src2.image, dst.image);
#endif

		// Value
		dst.value = src1.value;

		return RES_NEXT;
	}
};


Operator* getOperator(const std::string& name)
{
	if ( name.compare("AddI1Bg") == 0 ) {
		return new OpenCLOperator::Add();
	}
}

}// end of namespace OpenCLOperator
//-------------------------------------------------------------------
//-------------------------------------------------------------------
