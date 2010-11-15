/**********************************************************************
 *                                                                    *
 * tgt - Tiny Graphics Toolbox                                        *
 *                                                                    *
 * Copyright (C) 2006-2008 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the tgt library. This library is free         *
 * software; you can redistribute it and/or modify it under the terms *
 * of the GNU Lesser General Public License version 2.1 as published  *
 * by the Free Software Foundation.                                   *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU Lesser General Public License for more details.                *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License in the file "LICENSE.txt" along with this library.         *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 **********************************************************************/

#ifdef TGT_HAS_FFMPEG

#include "videoencoder.h"

#include "tgt/logmanager.h"
#include "tgt/vector.h"

#include <sstream>

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

namespace tgt {

namespace {

enum PixelOrderManipulation {
    NONE, FLIP_VERT, REVERSE
};

/**
 * @see ContainerCodecPair
 * @see VideoEncoder#getContainerCodecPairNames()
 */
const char *containerCodecPairNames[] = { "auto", "mpeg4 in avi", "wmv in wmv",
        "flv in flv", "huffyuv in avi", "ogg in ogg" };

/**
 * float to uint8_t conversion
 * TODO we have to put down an assumption about the pixel-order expected or let it be specified for the input data
 * @param pixels as floats
 * @param size number of pixels
 * @param lineSize (ignored but for FLIP_VERT)
 * @param manip (defaults: NONE)
 * @return pixels of type uint8_t
 */
uint8_t* floatToUint8_tRGB(float* pixels, const int size, const int lineSize,
        enum PixelOrderManipulation manip = NONE) {
    tgt::vec4* pixels_f = reinterpret_cast<tgt::vec4*> (pixels);
    tgt::col3* pixels_b = new tgt::col3[size];
    int j = 0;
    for (int i = 0; i < size; i++) {
        /*
         * 0 ... lineSize-1
         * lineSize ... 2*lineSize-1
         * ...
         * size-lineSize ... size-1
         */
        switch (manip) {
        case NONE:
            j = i; // up to compiler optimisation
            break;
        case REVERSE:
            j = size - i - 1;
            break;
        case FLIP_VERT:
            int curLine = (size / lineSize) - (i / lineSize) - 1;
            j = curLine * lineSize + (i % lineSize); // assume int div truncates
            break;
        }

        // dropping alpha channel and clamp to [0.0; 1.0]
        pixels_b[i] = tgt::clamp(pixels_f[j].xyz(), 0.f, 1.f) * 255.f;
    }
    return reinterpret_cast<uint8_t*> (pixels_b);
}

/**
 * uint16_t to uint8_t conversion
 * TODO we have to put down an assumption about the pixel-order expected or let it be specified for the input data
 * @param pixels as uint16_t
 * @param size number of pixels
 * @param lineSize (ignored but for FLIP_VERT)
 * @param manip (defaults: NONE)
 * @return pixels of type uint8_t
 */
uint8_t* uint16ToUint8_tRGB(uint16_t* pixels, const int size, const int lineSize,
        enum PixelOrderManipulation manip = NONE) {
    tgt::Vector4<uint16_t>* pixels_16 = reinterpret_cast<tgt::Vector4<uint16_t>*> (pixels);
    tgt::col3* pixels_b = new tgt::col3[size];
    int j = 0;
    for (int i = 0; i < size; i++) {
        /*
         * 0 ... lineSize-1
         * lineSize ... 2*lineSize-1
         * ...
         * size-lineSize ... size-1
         */
        switch (manip) {
        case NONE:
            j = i; // up to compiler optimisation
            break;
        case REVERSE:
            j = size - i - 1;
            break;
        case FLIP_VERT:
            int curLine = (size / lineSize) - (i / lineSize) - 1;
            j = curLine * lineSize + (i % lineSize); // assume int div truncates
            break;
        }

        // dropping alpha channel and converting to 8 bit
        pixels_b[i] = tgt::col3(pixels_16[j].x >> 8, pixels_16[j].y >> 8, pixels_16[j].z >> 8);
    }
    return reinterpret_cast<uint8_t*> (pixels_b);
}

/**
 * uint8_t RGBA to uint8_t RGB conversion
 * TODO we have to put down an assumption about the pixel-order expected or let it be specified for the input data
 * @param pixels as uint8_t
 * @param size number of pixels
 * @param lineSize (ignored but for FLIP_VERT)
 * @param manip (defaults: NONE)
 * @return pixels of type uint8_t
 */
uint8_t* uint8ToUint8_tRGB(uint8_t* pixels, const int size, const int lineSize,
        enum PixelOrderManipulation manip = NONE) {
    tgt::col4* pixels_col = reinterpret_cast<tgt::col4*> (pixels);
    tgt::col3* pixels_b = new tgt::col3[size];
    int j = 0;
    for (int i = 0; i < size; i++) {
        /*
         * 0 ... lineSize-1
         * lineSize ... 2*lineSize-1
         * ...
         * size-lineSize ... size-1
         */
        switch (manip) {
        case NONE:
            j = i; // up to compiler optimisation
            break;
        case REVERSE:
            j = size - i - 1;
            break;
        case FLIP_VERT:
            int curLine = (size / lineSize) - (i / lineSize) - 1;
            j = curLine * lineSize + (i % lineSize); // assume int div truncates
            break;
        }

        // dropping alpha channel 
        pixels_b[i] = tgt::col3(pixels_col[j].x, pixels_col[j].y, pixels_col[j].z);
    }
    return reinterpret_cast<uint8_t*> (pixels_b);
}

/**
 * size of encoder's output-buffer and half of IO-buffer's size
 */
unsigned int videoOutBufferSize;

/*
 * some default codec parameters commonly used
 */
const int defaultGopSize = 10;
const int defaultMaxBFrame = 1;
const int minimalFps = 1;
const int minimalBitrate = 400000;

/*
 * containerAppendices
 * @see tgt::ContainerCodecPair
 * @see VEAVContext#initFormatContext(tgt::ContainerCodecPair)
 */
const char* mpegAppendix = "mpg";
const char* aviAppendix = "avi";
const char* flvAppendix = "flv";
const char* wmvAppendix = "wmv";
const char* oggAppendix = "ogg";

const char* containerAppendix(VideoEncoder::ContainerCodecPair preset) {
    switch (preset) {
    case VideoEncoder::MPEG4AVI:
    case VideoEncoder::HUFFYUVAVI:
        return aviAppendix;
    case VideoEncoder::WMVWMV:
        return wmvAppendix;
    case VideoEncoder::FLVFLV:
        return flvAppendix;
    case VideoEncoder::OGGTHEORA:
        return oggAppendix;
    default:
        return mpegAppendix;
    }
}

CodecID containerCodecId(VideoEncoder::ContainerCodecPair preset) {
    switch (preset) {
    default:
        return CODEC_ID_MPEG2VIDEO;
    case VideoEncoder::FLVFLV:
        return CODEC_ID_FLV1;
    case VideoEncoder::MPEG4AVI:
        return CODEC_ID_MPEG4;
    case VideoEncoder::WMVWMV:
        return CODEC_ID_WMV2;
    case VideoEncoder::HUFFYUVAVI:
        return CODEC_ID_HUFFYUV;
    case VideoEncoder::OGGTHEORA:
        return CODEC_ID_THEORA;
    }
}

PixelFormat codecPixFmt(VideoEncoder::ContainerCodecPair preset) {
    switch (preset) {
    default:
        return PIX_FMT_YUV420P;
    case VideoEncoder::HUFFYUVAVI:
        return PIX_FMT_YUV422P;
    }
}

struct VEAVContext {

    /**
     * log category for tgt-logging
     */
    std::string loggerCat_;

    // Encoder format-configuration and stream-state
    AVFormatContext* formatContext_;
    AVStream* videoStream;

    SwsContext* swsContext; // converts inputFrame to videoFrame, which is ready for encoding
    AVFrame* videoFrame; // encoded with videoFormat pix_fmt

    int frameCount;

    /**
     * encoder's output buffer
     * @see #videoOutBufferSize
     */
    uint8_t* videoBuffer;

    GLenum pixelType_; // input specification
    GLint pixelFormat_;

    /**
     * init picture- & video-buffer
     * @see #initCodec(int,int,int,int) should be run before
     */
    bool initBuffers() {
        AVCodecContext* codecContext = videoStream->codec;
        PixelFormat pixelFormat = codecContext->pix_fmt;
        const int width = codecContext->width;
        const int height = codecContext->height;

        // alloc frame/picture buffer
        videoFrame = avcodec_alloc_frame();
        videoOutBufferSize = avpicture_get_size(pixelFormat, width, height);
        uint8_t* pictureBuffer = static_cast<uint8_t*> (av_malloc(
                videoOutBufferSize));
        if (!pictureBuffer) {
            LERRORC(loggerCat_, "unable to alloc picture buffer");
            return false;
        }
        avpicture_fill(reinterpret_cast<AVPicture*> (videoFrame),
                pictureBuffer, pixelFormat, width, height);

        // alloc video buffer
        videoBuffer = static_cast<uint8_t*> (av_malloc(videoOutBufferSize));

        return true;
    }

    /**
     * init video codec
     */
    bool initCodec(VideoEncoder::ContainerCodecPair preset, const int bitRate,
                   const int width, const int height, const int fps)
    {
        AVCodecContext* codecContext = videoStream->codec;
        codecContext->codec_id = formatContext_->video_codec_id;
        codecContext->codec_type = CODEC_TYPE_VIDEO;

        codecContext->bit_rate = bitRate;
        codecContext->width = width;
        codecContext->height = height;

        codecContext->time_base.num = 1;
        codecContext->time_base.den = fps;

        codecContext->gop_size = defaultGopSize;
        codecContext->pix_fmt = codecPixFmt(preset);

        if (codecContext->codec_id == CODEC_ID_MPEG2VIDEO) {
            codecContext->max_b_frames = defaultMaxBFrame;
        }

        if (codecContext->codec_id == CODEC_ID_MPEG1VIDEO) {
            codecContext->mb_decision = FF_MB_DECISION_RD;
        }

        if (codecContext->codec_id == CODEC_ID_MPEG4) {
            const char xvid[] = "DX50";
            codecContext->codec_tag = (xvid[3] << 24) + (xvid[2] << 16)
                    + (xvid[1] << 8) + xvid[0];
        }

        codecContext->strict_std_compliance = FF_COMPLIANCE_VERY_STRICT; // we are the good ones

        AVCodec* codec = avcodec_find_encoder(codecContext->codec_id);

        if (!codec) {
            LERRORC(loggerCat_, "Could not find Codec " << codecContext->codec_id);
            return false;
        }

        if (avcodec_open(codecContext, codec) < 0) {
            LERRORC(loggerCat_, "Could not load Codec " << codec->id);
            return false;
        }

        return true;
        /* codecContext == videoStream->codec */
    }

    bool checkParams(const int fps, const int width, const int height,
            GLenum pixelFormat, GLenum pixelType, const int bitRate) {

        // TODO check limits of swscale conversion
        if (GL_RGBA != pixelFormat || (GL_FLOAT != pixelType && GL_UNSIGNED_SHORT != pixelType && GL_UNSIGNED_BYTE != pixelType)) {
            LERRORC(loggerCat_,"only GL_RGBA + GL_FLOAT/GL_UNSIGNED_SHORT/GL_UNSIGNED_BYTE as input supported");
            return false;
        }

        if (width <= 0 || height <= 0) {
            LERRORC(loggerCat_, "illegal dimensions: (" << width << "x" << height << ")");
            return false;
        }

        if ((width % 2) + (height % 2) != 0) {
            LERRORC(loggerCat_, "width and height must be multiple of 2 - " << width << ":" << height);
            return false;
        }

        if (fps < minimalFps) {
            LERRORC(loggerCat_, "low fps: " << fps << "; minimum: " << minimalFps);
            return false;
        }

        if (bitRate < minimalBitrate) {
            LERRORC(loggerCat_, "low bitrate: " << bitRate << "; minimum: " << minimalBitrate);
            return false;
        }

        return true;
    }

    std::string initFormatContext(VideoEncoder::ContainerCodecPair preset, std::string filePath) {

        // init format-context
        formatContext_ = avformat_alloc_context();
        formatContext_->audio_codec_id = CODEC_ID_NONE;

        // let ffmpeg guess what stream-format matches to container-format
        AVOutputFormat* outputFormat;

        if (preset == VideoEncoder::GUESS) {
            outputFormat = guess_format(0, filePath.c_str(), 0);

            if (!outputFormat) {
                LWARNINGC(loggerCat_, "Could not guess output format, using MPEG");
                outputFormat = guess_format(mpegAppendix, 0, 0);
                filePath.append(".");
                filePath.append(mpegAppendix);
            }

            if (!outputFormat) {
                LERRORC(loggerCat_, "Could not find suitable output format");
                return "";
            }
            formatContext_->oformat = outputFormat;
            formatContext_->video_codec_id = outputFormat->video_codec;
        } else {
            const char* appendix = containerAppendix(preset);
            outputFormat = guess_format(appendix, 0, 0);
            formatContext_->oformat = outputFormat;
            if (filePath.substr(filePath.size() - strlen(appendix) - 1)
                    == std::string(appendix)) {
                filePath.append(".");
                filePath.append(appendix);
            }
            formatContext_->video_codec_id = containerCodecId(preset);
        }
        return filePath;
    }

    VEAVContext(std::string filePath, const int fps, const int width,
                const int height, GLenum pixelFormat, GLenum pixelType,
                const int bitRate, VideoEncoder::ContainerCodecPair preset) throw (Exception)
    {
        // loggerCat_
        // std::string-s concat with "+", but iostream-string-s concat with "<<"
        loggerCat_ = "";
        loggerCat_ += "tgt.VideoEncoder.AVContext::";
        loggerCat_ += filePath;

#ifndef TGT_DEBUG
        //av_log_set_level(AV_LOG_QUIET);
#endif

        if (!checkParams(fps, width, height, pixelFormat, pixelType, bitRate)) {
            throw Exception("invalid parameters, see log for details");
        }

        //
        swsContext = 0;
        frameCount = 0;

        pixelFormat_ = pixelFormat;
        pixelType_ = pixelType;

        // avcodec init
        av_register_all();

        filePath = initFormatContext(preset, filePath);
        if (filePath.size() == 0) {
            LERRORC(loggerCat_,"could not determine formatContext");
            return;
        }

        // init video-stream
        if (formatContext_->video_codec_id != CODEC_ID_NONE) {
            videoStream = av_new_stream(formatContext_, 1);

            if (!videoStream) {
                throw Exception("Could not allocate stream");
            }
            if (!(initCodec(preset, bitRate, width, height, fps) && initBuffers())) {
                throw Exception("Could not init codec, see log for details");
            }
        } else {
            throw Exception("Failed to allocate format-context");
            return;
        }

        // must set parameters
        if (av_set_parameters(formatContext_, 0) < 0) {
            LERRORC(loggerCat_, "invalid format parameters");
            return;
        }

#ifdef TGT_DEBUG
        dump_format(formatContext_, 0, filePath.c_str(), 1);//outputs format specs
#endif

        // open video's file (will overwrite)
        if (url_fopen(&formatContext_->pb, filePath.c_str(), URL_WRONLY) < 0) {
            throw Exception("file " + filePath + " is not accessible for write-access");
        }

        /*
         * must be called before any I/O
         * double videoOutBufferSize for slow fs
         */
        url_setbufsize(formatContext_->pb, 2 * videoOutBufferSize);
        url_resetbuf(formatContext_->pb, URL_WRONLY);

        // write format's header (if any)
        av_write_header(formatContext_);
    }

    /**
     * assume frames are passed sequential
     */
    void nextFrame(uint8_t* pixels) {
        AVCodecContext* codecContext = videoStream->codec;

        // since pixels are filled inside, this is rather a lightweight allocation
        AVFrame* inputFrame = avcodec_alloc_frame();

        /*
         * fill input into inputFrame
         * inputFrame->data ^= pixels is freed outside this scope
         */
        avpicture_fill((AVPicture*) inputFrame, pixels, PIX_FMT_RGB24,
                codecContext->width, codecContext->height);

        // convert RGB24 -> YUV420P
        if (swsContext == 0) {
            swsContext = sws_getContext(codecContext->width,
                    codecContext->height, PIX_FMT_RGB24, codecContext->width,
                    codecContext->height, codecContext->pix_fmt, SWS_BICUBIC,
                    0, 0, 0);
        }
        sws_scale(swsContext, inputFrame->data, inputFrame->linesize, 0,
                codecContext->height, videoFrame->data, videoFrame->linesize);

        av_free(inputFrame);

        int outputSize = avcodec_encode_video(codecContext, videoBuffer,
                videoOutBufferSize, videoFrame);
        if (outputSize > 0) {
            AVPacket packet;
            av_init_packet(&packet);

            packet.pts = av_rescale_q(codecContext->coded_frame->pts,
                    codecContext->time_base, videoStream->time_base);
            if (codecContext->coded_frame->key_frame) {
                packet.flags |= PKT_FLAG_KEY;
            }
            packet.stream_index = videoStream->index;
            packet.data = videoBuffer;
            packet.size = outputSize;

            if (av_write_frame(formatContext_, &packet) != 0) {
                LWARNINGC(loggerCat_, "error while writing frame to file");
            }

            // this would free videoBuffer, we want to reuse
            //av_destruct_packet(&packet);
        }
        frameCount++;
    }

    /**
     * write file trailer and finish encoding
     */
    virtual ~VEAVContext() {
        avcodec_close(videoStream->codec);
        av_free(videoFrame->data[0]);
        av_free(videoFrame);

        av_free(videoBuffer);

        av_write_trailer(formatContext_);

        url_fclose(formatContext_->pb);

        av_free(formatContext_);
    }
};

VEAVContext* encoderContext;// singleton

} /* anonymous namespace */

std::string VideoEncoder::loggerCat_("tgt.VideoEncoder");

VideoEncoder::VideoEncoder() {
    encoderContext = 0;
    bitrate_ = 2000000;
    preset_ = GUESS;
}

VideoEncoder::~VideoEncoder() {
}

void VideoEncoder::startVideoEncoding(std::string filePath, const int fps,
                                      const int width, const int height,
                                      GLint pixelFormat, GLenum pixelType) throw (Exception) {
    if (encoderContext != 0) {
        LWARNING("already encoding a video");
        return;
    }

    // we must not reuse this context because it still stores state of finished encoding
    // TODO consider reusing everything but videoStream
    encoderContext = new VEAVContext(filePath, fps, width, height, pixelFormat,
            pixelType, bitrate_, preset_);
}

void VideoEncoder::stopVideoEncoding() {
    if (encoderContext == 0) {
        LERROR("Encoding has already aborted");
        return;
    }

    AVCodecContext* codecContext = encoderContext->videoStream->codec;

    if (encoderContext->frameCount % codecContext->time_base.den != 0) {
        LWARNING("passed frames doesn't match fps: " << encoderContext->frameCount << "/" << codecContext->time_base.den);
    }

    delete encoderContext;
    encoderContext = 0;
}

/**
 * TODO assumption of float not applied here cause we could specify pixelComponents' type when starting animation
 */
void VideoEncoder::nextFrame(GLvoid* pixels) {
    if (pixels == 0)
        return;

    AVCodecContext* codecContext = encoderContext->videoStream->codec;

    if (encoderContext->pixelFormat_ != GL_RGBA) {
        LERROR("Only GL_RGBA as input format supported.");
        return;
    }

    uint8_t* pixels8 = 0;
    if (encoderContext->pixelType_ == GL_FLOAT) {
        pixels8 = tgt::floatToUint8_tRGB(reinterpret_cast<float*>(pixels), codecContext->width
                    * codecContext->height, codecContext->width, FLIP_VERT);
    }
    else if (encoderContext->pixelType_ == GL_UNSIGNED_SHORT) {
        pixels8 = tgt::uint16ToUint8_tRGB(reinterpret_cast<uint16_t*>(pixels), codecContext->width
                    * codecContext->height, codecContext->width, FLIP_VERT); 
    }
    else if (encoderContext->pixelType_ == GL_UNSIGNED_BYTE) {
        pixels8 = tgt::uint8ToUint8_tRGB(reinterpret_cast<uint8_t*>(pixels), codecContext->width
                    * codecContext->height, codecContext->width, FLIP_VERT); 
    }
    else {
        LERROR("Unknown pixel type. Only GL_FLOAT/GL_UNSIGNED_SHORT/GL_UNSIGNED_BYTE supported");
        return;
    }

    encoderContext->nextFrame(pixels8);
    delete [] pixels8;
}

void VideoEncoder::setup(int preset, int bitrate) {
    preset_ = (ContainerCodecPair) preset;
    bitrate_ = bitrate;
}

std::vector<std::string> VideoEncoder::getSupportedFormatsByFileEnding() {
    std::vector<std::string> formats;
    formats.push_back("avi");
    formats.push_back("mpg");
    formats.push_back("wmv");
    formats.push_back("flv");
    formats.push_back("ogg");

    return formats;
}

std::vector<std::string> VideoEncoder::getSupportedFormatDescriptions() {
    std::vector<std::string> formats;
    formats.push_back("AVI video files");
    formats.push_back("MPEG video files");
    formats.push_back("WMV video files");
    formats.push_back("Flash FLV video files");
    formats.push_back("OGG video files");

    return formats;
}

const char** VideoEncoder::getContainerCodecPairNames() {
    return containerCodecPairNames;
}

/**
 * static
 */
const char* VideoEncoder::getContainerAppendix() {
    return containerAppendix(preset_);
}

} // namespace tgt

#endif // TGT_HAS_FFMPEG
