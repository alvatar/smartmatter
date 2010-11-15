/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2010 The Voreen Team. <http://www.voreen.org>   *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#ifndef VRN_CLWRAPPER_H
#define VRN_CLWRAPPER_H

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "tgt/vector.h"
#include "tgt/texture.h"

namespace voreen {

namespace cl {

std::string clErrorToString(cl_int err);
cl_int _lCLError(cl_int err, int line, const char* file);
#define LCL_ERROR(err) _lCLError(err, __LINE__, __FILE__)

class Device;
class Platform;
class Kernel;
class Buffer;
class MemoryObject;

void contextCallback(const char *errinfo, const void* private_info, size_t cb, void* user_data);

//---------------------------------------------------------

class OpenCL {
public:
    OpenCL();
    const std::vector<Platform>& getPlatforms() const { return platforms_; }
    ///Needs CL extension, doesn´t link
    static Device* getCurrentDeviceForGlContext();
    ///Needs CL extension, doesn´t link
    static std::vector<Device*> getDevicesForGlContext();
    /// Computes an appropriate global work size as the multiple
    /// of local workSize that is nearest (greater) to numItems
    static size_t shrRoundUp(size_t localWorkSize, size_t numItems);

protected:
    std::vector<Platform> platforms_;

    static const std::string loggerCat_;
};

//---------------------------------------------------------

class Platform {
public:
    enum Profile {
        FULL_PROFILE = 0,
        EMBEDDED_PROFILE,
        UNKNOWN
    };

    /**
     * Specifies the version of the OpenCL implementation.
     * CL_VERSION_x_y denotes OpenCL version x.y.
     */
    class ClVersion {
        public:
        ClVersion(int major = 0, int minor = 0);

        /**
         * Parse OpenCL version string as specified:
         *
         * The CL_PLATFORM_VERSION strings begins with "OpenCL " and is followed by a version number.
         * The version number uses the following format:
         *      major_version.minor_version
         *
         * Vendor-specific information may follow the version number.
         * Its format depends on the implementation, but a space always separates the version number and the vendor-specific information.
         */
        bool parseVersionString(const std::string& st);

        int major_;
        int minor_;

        int major() const { return major_; }
        int minor() const { return minor_; }

        friend bool operator==(const ClVersion& x, const ClVersion& y);
        friend bool operator!=(const ClVersion& x, const ClVersion& y);
        friend bool operator<(const ClVersion& x, const ClVersion& y);
        friend bool operator<=(const ClVersion& x, const ClVersion& y);
        friend bool operator>(const ClVersion& x, const ClVersion& y);
        friend bool operator>=(const ClVersion& x, const ClVersion& y);
        friend std::ostream& operator<<(std::ostream& s, const ClVersion& v);

        static const ClVersion VRN_CL_VERSION_1_0;
    };

    Platform(cl_platform_id id);

    const std::vector<Device*>& getDevices() const { return devices_; }
    Profile getProfile() const { return profile_; }
    std::string getName() const { return name_; }
    std::string getVendor() const { return vendor_; }
    std::string getVersionString() const { return versionString_; }
    std::string getExtensionString() const { return extensionString_; }
    const std::set<std::string>& getExtensions() const { return extensions_; }
    bool isExtensionSupported(std::string ext) const;
    bool supportsGlSharing() const { return isExtensionSupported("cl_khr_gl_sharing"); }

    template<class T>
    T getInfo(cl_platform_info info) const {
        T ret;
        LCL_ERROR(clGetDeviceInfo(id_, info, sizeof(ret), &ret, 0));
        return ret;
    }
protected:

    cl_platform_id id_;

    //info:
    Profile profile_;
    std::string name_;
    std::string vendor_;
    std::string extensionString_;
    std::set<std::string> extensions_;
    std::string versionString_;
    ClVersion version_;

    std::vector<Device*> devices_;

    static const std::string loggerCat_;
};

//template specialization for strings:
template<>
std::string Platform::getInfo(cl_platform_info info) const;

//---------------------------------------------------------

class Device {
public:
    Device(cl_device_id id);

    cl_device_id getId() const { return id_; }

    template<class T>
    T getInfo(cl_device_info info) const {
        T ret;
        LCL_ERROR(clGetDeviceInfo(id_, info, sizeof(ret), &ret, 0));
        return ret;
    }

    std::string getName() const { return name_; }
    std::string getExtensionString() const { return extensionString_; }
    const std::set<std::string>& getExtensions() const { return extensions_; }
    bool isExtensionSupported(std::string ext) const;
    bool supportsGlSharing() const { return isExtensionSupported("cl_khr_gl_sharing"); }
    cl_uint getMaxWorkGroupSize() const;
protected:
    cl_device_id id_;

    std::string name_;
    std::string extensionString_;
    std::set<std::string> extensions_;
    tgt::ivec2 maxImageSize2D_;
    tgt::ivec3 maxImageSize3D_;

    static const std::string loggerCat_;
};

//template specialization for strings:
template<>
std::string Device::getInfo(cl_device_info info) const;

//---------------------------------------------------------

struct ContextProperty {
    cl_context_properties name_;
    cl_context_properties value_;
    ContextProperty(cl_context_properties name, cl_context_properties value) : name_(name), value_(value) {}
};

class Context {
public:
    Context(const Device* device);
    Context(const std::vector<ContextProperty>& properties, const Device* device);
    //TODO:
    //Context(const std::vector<Device>& devices);
    //Context(const Platform& platform, const std::vector<Device>& devices);
    //Context(const Platform& platform, const Device& device);
    //TODO: create context from type
    //TODO: additional context properties
    //TODO: notification functions, user data
    ~Context();

    ///Tested for Linux, code for windows is untested, code for apple is commented out
    static std::vector<ContextProperty> generateGlSharingProperties();

    cl_context getId() const { return id_; }

    template<class T>
    T getInfo(cl_context_info info) const {
        T ret;
        LCL_ERROR(clGetContextInfo(id_, info, sizeof(ret), &ret, 0));
        return ret;
    }

    //TODO: getDevices()
    //TODO: getContextProperties()
protected:
    cl_context id_;

    static const std::string loggerCat_;
};

//template specialization for strings:
template<>
std::string Context::getInfo(cl_context_info info) const;

//---------------------------------------------------------

class Event {
public:
    Event(cl_event id) : id_(id) {
        //if(id_)
            //LCL_ERROR(clRetainEvent(id_));
    }

    Event(const Event& e) {
        id_ = e.getId();
        //if(id_)
            //LCL_ERROR(clRetainEvent(id_));
    }

    ~Event() {
        //if(id_)
            //LCL_ERROR(clReleaseEvent(id_));
    }

    void wait() const { LCL_ERROR(clWaitForEvents(1, &id_)); }

    template<class T>
    T getInfo(cl_event_info info) const {
        T ret;
        LCL_ERROR(clGetEventInfo(id_, info, sizeof(ret), &ret, 0));
        return ret;
    }

    ///Possible values for info: CL_PROFILING_COMMAND_QUEUED CL_PROFILING_COMMAND_SUBMIT CL_PROFILING_COMMAND_START CL_PROFILING_COMMAND_END
    cl_ulong getProfilingInfo(cl_profiling_info info) const {
        cl_ulong ret;
        LCL_ERROR(clGetEventProfilingInfo(id_, info, sizeof(cl_ulong), &ret, 0));
        return ret;
    }

    cl_ulong getProfilingStartToEnd() const { return (getProfilingInfo(CL_PROFILING_COMMAND_END) - getProfilingInfo(CL_PROFILING_COMMAND_START)); }

    /**
     * Return the execution status of the command identified by event. The valid values are:
     * CL_QUEUED (command has been enqueued in the command-queue),
     * CL_SUBMITTED (enqueued command has been submitted by the host to the device associated with the command-queue),
     * CL_RUNNING (device is currently executing this command),
     * CL_COMPLETE (the command has completed), or Error code given by a negative integer value.
     */
    cl_int getExecutionStatus() {
        return getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS);
    }

    /**
     * Return the command associated with event. Can be one of the following values:
     * CL_COMMAND_NDRANGE_KERNEL
     * CL_COMMAND_TASK
     * CL_COMMAND_NATIVE_KERNEL
     * CL_COMMAND_READ_BUFFER
     * CL_COMMAND_WRITE_BUFFER
     * CL_COMMAND_COPY_BUFFER
     * CL_COMMAND_READ_IMAGE
     * CL_COMMAND_WRITE_IMAGE
     * CL_COMMAND_COPY_IMAGE
     * CL_COMMAND_COPY_BUFFER_TO_IMAGE
     * CL_COMMAND_COPY_IMAGE_TO_BUFFER
     * CL_COMMAND_MAP_BUFFER
     * CL_COMMAND_MAP_IMAGE
     * CL_COMMAND_UNMAP_MEM_OBJECT
     * CL_COMMAND_MARKER
     * CL_COMMAND_ACQUIRE_GL_OBJECTS
     * CL_COMMAND_RELEASE_GL_OBJECTS
     */
    cl_command_type getCommandType() {
        return getInfo<cl_command_type>(CL_EVENT_COMMAND_TYPE);
    }

    cl_event getId() const { return id_; }
protected:
    cl_event id_;
};

//---------------------------------------------------------

class CommandQueue {
public:
    CommandQueue(const Context* context, const Device* device, cl_command_queue_properties properties = 0);
    ~CommandQueue();

    //TODO: set command queue properties
    cl_command_queue_properties getProperties() const { return properties_; }
    cl_int flush() { return LCL_ERROR(clFlush(id_)); }
    cl_int finish() { return LCL_ERROR(clFinish(id_)); }

    Event enqueue(const Kernel* kernel);
    Event enqueue(const Kernel* kernel, size_t globalWorkSize, size_t localWorkSizes);
    Event enqueue(const Kernel* kernel, size_t globalWorkSize);
    Event enqueue(const Kernel* kernel, tgt::svec2 globalWorkSizes, tgt::svec2 localWorkSizes);
    Event enqueue(const Kernel* kernel, tgt::svec2 globalWorkSizes);
    Event enqueue(const Kernel* kernel, tgt::svec3 globalWorkSizes, tgt::svec3 localWorkSizes);
    Event enqueue(const Kernel* kernel, tgt::svec3 globalWorkSizes);
    Event enqueue(const Kernel* kernel, const std::vector<size_t>& globalWorkSizes, const std::vector<size_t>& localWorkSizes);
    //TODO: add method with offsets
    //TODO: wrap clEnqueueNativeKernel

    Event enqueueRead(const Buffer* buffer, void* data, bool blocking = true);
    Event enqueueWrite(const Buffer* buffer, void* data, bool blocking = true);
    //TODO: add methods with offset + size
    //TODO: wrap clEnqueueCopyBuffer

    Event enqueueAcquireGLObject(const MemoryObject* obj);
    Event enqueueReleaseGLObject(const MemoryObject* obj);

    Event enqueueMarker();
    void enqueueBarrier();
    void enqueueWaitForEvent(const Event* event);
    void enqueueWaitForEvents(const std::vector<Event*>& event);

    template<class T>
    T getInfo(cl_command_queue_info info) const {
        T ret;
        LCL_ERROR(clGetCommandQueueInfo(id_, info, sizeof(ret), &ret, 0));
        return ret;
    }
protected:
    cl_command_queue id_;
    cl_command_queue_properties properties_;

    static const std::string loggerCat_;
};

//template specialization for strings:
template<>
std::string CommandQueue::getInfo(cl_command_queue_info info) const;

//---------------------------------------------------------

class Program {
public:
    Program(const Context& context);
    Program(const Context* context);
    ~Program();

    void setSource(const std::string& source);
    void setSource(const std::vector<std::string>& source);
    bool loadSource(const std::string& filename);
    bool loadSource(const std::vector<std::string>& filenames);
    ///Reload source from disk (if program has been loaded from disk)
    bool reload();

    std::string getBuildOptions() const { return buildOptions_; }
    ///This does NOT automatically recompile the program.
    void setBuildOptions(const std::string& buildOptions) { buildOptions_ = buildOptions; }

    bool build();
    bool build(const Device* device);
    bool build(const std::vector<Device*>& devices);

    /**
     * Get a Kernel.
     * \warning Do not save this pointer to use it later. Kernels get invalid and are deleted if the Program is rebuild.
     *
     * @param name Name of the Kernel function
     *
     * @return The Kernel named 'name' or 0 if no such Kernel exists or the Program is invalid or not build.
     */
    Kernel* getKernel(const std::string& name);

    std::string getBuildLog(const Device* device) const;
    ///Returns one of the following: CL_BUILD_NONE CL_BUILD_ERROR CL_BUILD_SUCCESS CL_BUILD_IN_PROGRESS
    cl_build_status getBuildStatus(const Device* device) const;
    std::string getBuildOptions(const Device* device) const;

    template<class T>
    T getInfo(cl_program_info info) const {
        T ret;
        LCL_ERROR(clGetProgramdInfo(id_, info, sizeof(ret), &ret, 0));
        return ret;
    }

    //TODO: get binaries
    template<class T>
    T getBuildInfo(const Device* device, cl_program_build_info info) const {
        T ret;
        LCL_ERROR(clGetProgramBuildInfo(id_, device->getId(), info, sizeof(ret), &ret, 0));
        return ret;
    }

    static std::string statusToString(cl_build_status status);

    cl_program getId() const { return id_; }
protected:

    void createWithSource();
    void createKernels();
    void clearKernels();
    void releaseProgram();

    cl_program id_;
    const Context* context_;

    std::string buildOptions_;
    std::vector<std::string> source_;
    std::vector<std::string> filenames_;
    std::map<std::string, Kernel*> kernels_;
    static const std::string loggerCat_;
};

//template specialization for strings:
template<>
std::string Program::getBuildInfo(const Device* device, cl_program_build_info info) const;

template<>
std::string Program::getInfo(cl_program_info info) const;

//---------------------------------------------------------

class MemoryObject {
public:
    MemoryObject();
    virtual ~MemoryObject();

    cl_mem getId() const { return id_; }
protected:
    cl_mem id_;
};

//---------------------------------------------------------

class Buffer : public MemoryObject {
public:
    ///flags: CL_MEM_READ_WRITE CL_MEM_WRITE_ONLY CL_MEM_READ_ONLY CL_MEM_USE_HOST_PTR CL_MEM_ALLOC_HOST_PTR CL_MEM_COPY_HOST_PTR
    Buffer(const Context& context, cl_mem_flags flags, size_t size, void* hostPtr = 0);
    Buffer(const Context* context, cl_mem_flags flags, size_t size, void* hostPtr = 0);
    virtual ~Buffer();

    size_t getSize() const { return size_; }
protected:
    size_t size_;
};

//---------------------------------------------------------

class SharedTexture : public MemoryObject {
public:
    ///flags: CL_MEM_READ_WRITE CL_MEM_WRITE_ONLY CL_MEM_READ_ONLY CL_MEM_USE_HOST_PTR CL_MEM_ALLOC_HOST_PTR CL_MEM_COPY_HOST_PTR
    SharedTexture(const Context* context, cl_mem_flags flags, tgt::Texture* tex);
    virtual ~SharedTexture();

    size_t getSize() const { return size_; }
protected:
    size_t size_;
    tgt::Texture* tex_;
};

//---------------------------------------------------------

class Sampler {
public:
    /**
     * @param context Must be a valid OpenCL context.
     * @param normalized_coords Determines if the image coordinates specified are normalized (if normalized_coords is CL_TRUE) or not (if normalized_coords is CL_FALSE).
     * @param addressing_mode Specifies how out-of-range image coordinates are handled when reading from an image. This can be set to CL_ADDRESS_REPEAT, CL_ADDRESS_CLAMP_TO_EDGE, CL_ADDRESS_CLAMP, and CL_ADDRESS_NONE.
     * @param filtering_mode Specifies the type of filter that must be applied when reading an image. This can be CL_FILTER_NEAREST or CL_FILTER_LINEAR.
     */
    Sampler(const Context* context, cl_bool normalized_coords, cl_addressing_mode addressing_mode, cl_filter_mode filter_mode);
    ~Sampler();

    cl_sampler getId() const { return id_; }

    template<class T>
    T getInfo(cl_sampler_info info) const {
        T ret;
        LCL_ERROR(clGetSamplerInfo(id_, info, sizeof(ret), &ret, 0));
        return ret;
    }

    ///Return the value specified by addressing_mode argument to clCreateSampler.
    cl_addressing_mode getAdressingMode() { return getInfo<cl_addressing_mode>(CL_SAMPLER_ADDRESSING_MODE); }

    ///Return the value specified by addressing_mode argument to clCreateSampler.
    cl_filter_mode getFilterMode() { return getInfo<cl_filter_mode>(CL_SAMPLER_FILTER_MODE); }

    ///Return the value specified by filter_mode argument to clCreateSampler.
    cl_bool    getNormalizedCoords() { return getInfo<cl_bool>(CL_SAMPLER_NORMALIZED_COORDS); }

protected:
    cl_sampler id_;
};

//---------------------------------------------------------

class Kernel {
public:
    Kernel(cl_kernel id);
    ~Kernel();

    ///Be sure this does what you want!
    template<class T>
    cl_int setArgTemplate(cl_uint index, const T& data) {
        return LCL_ERROR(clSetKernelArg(id_, index, sizeof(data), &data));
    }

    cl_int setArg(cl_uint index, float data) { return setArgTemplate(index, data); }

    cl_int setArg(cl_uint index, int data) { return setArgTemplate(index, data); }

    cl_int setArg(cl_uint index, uint data) { return setArgTemplate(index, data); }

#ifndef WIN32
    cl_int setArg(cl_uint index, size_t data) { return setArgTemplate(index, data); }
#endif

    cl_int setArg(cl_uint index, tgt::vec4 data) {
        return LCL_ERROR(clSetKernelArg(id_, index, sizeof(float)*4, data.elem));
       }

    cl_int setArg(cl_uint index, tgt::vec3 data) {
        return LCL_ERROR(setArg(index, tgt::vec4(data, 0.0f) ));
       }

    cl_int setArg(cl_uint index, tgt::vec2 data) {
        return LCL_ERROR(clSetKernelArg(id_, index, sizeof(float)*2, data.elem));
       }

    cl_int setArg(cl_uint index, tgt::ivec2 data) {
        return LCL_ERROR(clSetKernelArg(id_, index, sizeof(int)*2, data.elem));
       }

    cl_int setArg(cl_uint index, const MemoryObject* memObj) {
        cl_mem id = memObj->getId();
        return LCL_ERROR(clSetKernelArg(id_, index, sizeof(cl_mem), &id));
    }

    cl_int setArg(cl_uint index, const MemoryObject& memObj) {
        cl_mem id = memObj.getId();
        return LCL_ERROR(clSetKernelArg(id_, index, sizeof(cl_mem), &id));
    }

    cl_int setArg(cl_uint index, const Sampler* sampler) {
        cl_sampler id = sampler->getId();
        return LCL_ERROR(clSetKernelArg(id_, index, sizeof(cl_sampler), &id));
    }

    cl_int setArg(cl_uint index, const Sampler& sampler) {
        cl_sampler id = sampler.getId();
        return LCL_ERROR(clSetKernelArg(id_, index, sizeof(cl_sampler), &id));
    }

    cl_int setArg(cl_uint index, size_t size, const void* data) {
        return LCL_ERROR(clSetKernelArg(id_, index, size, data));
    }

    template<class T>
    T getInfo(cl_kernel_info info) const {
        T ret;
        LCL_ERROR(clGetKernelInfo(id_, info, sizeof(ret), &ret, 0));
        return ret;
    }

    template<class T>
    T getWorkGroupInfo(cl_kernel_work_group_info info, const Device& device) const{
        T ret;
        LCL_ERROR(clGetKernelWorkGroupInfo(id_, device.getId(), info, sizeof(ret), &ret, 0));
        return ret;
    }

    cl_kernel getId() const { return id_; }
    cl_uint getNumArgs() const;
    cl_uint getWorkGroupSize(const Device& device) const;
    std::string getName() const;
protected:
    cl_kernel id_;

    static const std::string loggerCat_;
};

//template specialization for strings:
template<>
std::string Kernel::getInfo(cl_kernel_info info) const;

template<>
std::string Kernel::getWorkGroupInfo(cl_kernel_work_group_info info, const Device& device) const;


//-----------------------------------------------------------------------------------------------
//Some useful typedefs:

typedef tgt::vec4 float4;
typedef tgt::vec2 float2;

} //namespace cl

} //namespace voreen

#endif // VRN_CLWRAPPER_H
