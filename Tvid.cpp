// Continuing from where the code left off in ProjectManager::addAudioClip

            timeline.audioTracks.push_back(clip);
            timeline.duration = std::max(timeline.duration, startTime + duration);
            
            isDirty = true;
            LOG_INFO("Audio clip added: " + id);
            return id;
        }
        
        LOG_ERROR("Failed to add audio clip: " + filePath);
        return "";
    }
    
    bool removeClip(const std::string& clipId) {
        std::lock_guard<std::mutex> lock(projectMutex);
        
        // Remove from video tracks
        auto videoIt = std::remove_if(timeline.videoTracks.begin(), timeline.videoTracks.end(),
            [&clipId](const std::shared_ptr<VideoClip>& clip) {
                return clip->id == clipId;
            });
        
        if (videoIt != timeline.videoTracks.end()) {
            timeline.videoTracks.erase(videoIt, timeline.videoTracks.end());
            isDirty = true;
            LOG_INFO("Removed video clip: " + clipId);
            return true;
        }
        
        // Remove from audio tracks
        auto audioIt = std::remove_if(timeline.audioTracks.begin(), timeline.audioTracks.end(),
            [&clipId](const std::shared_ptr<AudioClip>& clip) {
                return clip->id == clipId;
            });
        
        if (audioIt != timeline.audioTracks.end()) {
            timeline.audioTracks.erase(audioIt, timeline.audioTracks.end());
            isDirty = true;
            LOG_INFO("Removed audio clip: " + clipId);
            return true;
        }
        
        LOG_WARNING("Clip not found for removal: " + clipId);
        return false;
    }
    
    bool updateClip(const std::string& clipId, const Json::Value& updates) {
        std::lock_guard<std::mutex> lock(projectMutex);
        
        // Update video clip
        for (auto& clip : timeline.videoTracks) {
            if (clip->id == clipId) {
                if (updates.isMember("startTime")) clip->startTime = updates["startTime"].asDouble();
                if (updates.isMember("duration")) clip->duration = updates["duration"].asDouble();
                if (updates.isMember("inPoint")) clip->inPoint = updates["inPoint"].asDouble();
                if (updates.isMember("outPoint")) clip->outPoint = updates["outPoint"].asDouble();
                if (updates.isMember("enabled")) clip->enabled = updates["enabled"].asBool();
                if (updates.isMember("opacity")) clip->opacity = updates["opacity"].asFloat();
                
                isDirty = true;
                LOG_INFO("Updated video clip: " + clipId);
                return true;
            }
        }
        
        // Update audio clip
        for (auto& clip : timeline.audioTracks) {
            if (clip->id == clipId) {
                if (updates.isMember("startTime")) clip->startTime = updates["startTime"].asDouble();
                if (updates.isMember("duration")) clip->duration = updates["duration"].asDouble();
                if (updates.isMember("volume")) clip->volume = updates["volume"].asFloat();
                if (updates.isMember("enabled")) clip->enabled = updates["enabled"].asBool();
                if (updates.isMember("muted")) clip->muted = updates["muted"].asBool();
                
                isDirty = true;
                LOG_INFO("Updated audio clip: " + clipId);
                return true;
            }
        }
        
        LOG_WARNING("Clip not found for update: " + clipId);
        return false;
    }
    
    Timeline& getTimeline() { 
        std::lock_guard<std::mutex> lock(projectMutex);
        return timeline; 
    }
    
    const Json::Value& getProjectData() const { 
        return projectData; 
    }
    
    bool isDirtyProject() const { 
        return isDirty; 
    }
    
    std::chrono::system_clock::time_point getLastSaveTime() const { 
        return lastSave; 
    }
    
    Json::Value getProjectInfo() const {
        std::lock_guard<std::mutex> lock(projectMutex);
        
        Json::Value info;
        info["name"] = timeline.name;
        info["duration"] = timeline.duration;
        info["width"] = timeline.width;
        info["height"] = timeline.height;
        info["frameRate"] = timeline.frameRate;
        info["videoTrackCount"] = static_cast<int>(timeline.videoTracks.size());
        info["audioTrackCount"] = static_cast<int>(timeline.audioTracks.size());
        info["isDirty"] = isDirty;
        info["lastSave"] = std::chrono::duration_cast<std::chrono::seconds>(
            lastSave.time_since_epoch()).count();
        
        return info;
    }
};

// Render engine for final video export
class RenderEngine {
public:
    struct ExportSettings {
        std::string outputPath;
        std::string videoCodec;
        std::string audioCodec;
        int width;
        int height;
        double frameRate;
        int videoBitrate;
        int audioBitrate;
        int audioSampleRate;
        std::string preset;
        int crf;
        std::string pixelFormat;
        bool hardwareAcceleration;
        
        ExportSettings() : videoCodec("libx264"), audioCodec("aac"), width(1920), height(1080), 
                         frameRate(30.0), videoBitrate(10000000), audioBitrate(192000), 
                         audioSampleRate(44100), preset("medium"), crf(23), 
                         pixelFormat("yuv420p"), hardwareAcceleration(true) {}
    };
    
    struct RenderProgress {
        int currentFrame;
        int totalFrames;
        double percentage;
        double estimatedTimeRemaining;
        std::string currentOperation;
        bool isComplete;
        bool hasError;
        std::string errorMessage;
        
        RenderProgress() : currentFrame(0), totalFrames(0), percentage(0.0), 
                         estimatedTimeRemaining(0.0), isComplete(false), hasError(false) {}
    };
    
private:
    VideoEngine videoEngine;
    AudioEngine audioEngine;
    EffectProcessor effectProcessor;
    std::atomic<bool> shouldCancel;
    RenderProgress currentProgress;
    std::mutex progressMutex;
    std::function<void(const RenderProgress&)> progressCallback;
    
public:
    RenderEngine() : shouldCancel(false) {
        LOG_DEBUG("RenderEngine initialized");
    }
    
    ~RenderEngine() {
        LOG_DEBUG("RenderEngine destroyed");
    }
    
    void setProgressCallback(std::function<void(const RenderProgress&)> callback) {
        progressCallback = callback;
    }
    
    bool exportVideo(const Timeline& timeline, const ExportSettings& settings) {
        LOG_INFO("Starting video export to: " + settings.outputPath);
        
        shouldCancel = false;
        updateProgress("Initializing export...", 0, 0, 0.0);
        
        // Initialize FFmpeg muxer
        AVFormatContext* outputFormat = nullptr;
        int ret = avformat_alloc_output_context2(&outputFormat, nullptr, nullptr, settings.outputPath.c_str());
        if (ret < 0) {
            setError("Could not create output context");
            return false;
        }
        
        // Add video stream
        AVStream* videoStream = avformat_new_stream(outputFormat, nullptr);
        if (!videoStream) {
            setError("Could not create video stream");
            avformat_free_context(outputFormat);
            return false;
        }
        
        AVCodecContext* videoCodecCtx = setupVideoEncoder(videoStream, settings);
        if (!videoCodecCtx) {
            setError("Could not setup video encoder");
            avformat_free_context(outputFormat);
            return false;
        }
        
        // Add audio stream
        AVStream* audioStream = avformat_new_stream(outputFormat, nullptr);
        if (!audioStream) {
            setError("Could not create audio stream");
            avcodec_free_context(&videoCodecCtx);
            avformat_free_context(outputFormat);
            return false;
        }
        
        AVCodecContext* audioCodecCtx = setupAudioEncoder(audioStream, settings);
        if (!audioCodecCtx) {
            setError("Could not setup audio encoder");
            avcodec_free_context(&videoCodecCtx);
            avformat_free_context(outputFormat);
            return false;
        }
        
        // Open output file
        if (!(outputFormat->oformat->flags & AVFMT_NOFILE)) {
            ret = avio_open(&outputFormat->pb, settings.outputPath.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0) {
                setError("Could not open output file: " + settings.outputPath);
                cleanup(outputFormat, videoCodecCtx, audioCodecCtx);
                return false;
            }
        }
        
        // Write header
        ret = avformat_write_header(outputFormat, nullptr);
        if (ret < 0) {
            setError("Error writing header");
            cleanup(outputFormat, videoCodecCtx, audioCodecCtx);
            return false;
        }
        
        // Calculate frame information
        double frameDuration = 1.0 / settings.frameRate;
        int totalFrames = static_cast<int>(timeline.duration * settings.frameRate);
        
        updateProgress("Rendering frames...", 0, totalFrames, 0.0);
        
        auto startTime = std::chrono::steady_clock::now();
        
        // Render frames
        for (int frameNumber = 0; frameNumber < totalFrames && !shouldCancel; frameNumber++) {
            double currentTime = frameNumber * frameDuration;
            
            // Render video frame
            cv::Mat frame = renderVideoFrame(timeline, currentTime, settings.width, settings.height);
            if (!frame.empty()) {
                if (!writeVideoFrame(outputFormat, videoCodecCtx, videoStream, frame, frameNumber)) {
                    setError("Error writing video frame " + std::to_string(frameNumber));
                    cleanup(outputFormat, videoCodecCtx, audioCodecCtx);
                    return false;
                }
            }
            
            // Render audio samples for this frame
            std::vector<float> audioSamples = renderAudioSamples(timeline, currentTime, frameDuration, settings.audioSampleRate);
            if (!audioSamples.empty()) {
                if (!writeAudioSamples(outputFormat, audioCodecCtx, audioStream, audioSamples, frameNumber)) {
                    setError("Error writing audio samples for frame " + std::to_string(frameNumber));
                    cleanup(outputFormat, videoCodecCtx, audioCodecCtx);
                    return false;
                }
            }
            
            // Update progress
            auto elapsed = std::chrono::steady_clock::now() - startTime;
            double elapsedSeconds = std::chrono::duration<double>(elapsed).count();
            double estimatedTotal = elapsedSeconds * totalFrames / (frameNumber + 1);
            double remaining = estimatedTotal - elapsedSeconds;
            
            updateProgress("Rendering frame " + std::to_string(frameNumber + 1) + "/" + std::to_string(totalFrames),
                         frameNumber + 1, totalFrames, remaining);
        }
        
        if (shouldCancel) {
            updateProgress("Export cancelled", 0, 0, 0.0);
            cleanup(outputFormat, videoCodecCtx, audioCodecCtx);
            // Remove incomplete file
            std::remove(settings.outputPath.c_str());
            return false;
        }
        
        updateProgress("Finalizing export...", totalFrames, totalFrames, 0.0);
        
        // Flush encoders
        flushEncoder(outputFormat, videoCodecCtx, videoStream);
        flushEncoder(outputFormat, audioCodecCtx, audioStream);
        
        // Write trailer
        ret = av_write_trailer(outputFormat);
        if (ret < 0) {
            setError("Error writing trailer");
            cleanup(outputFormat, videoCodecCtx, audioCodecCtx);
            return false;
        }
        
        // Cleanup
        cleanup(outputFormat, videoCodecCtx, audioCodecCtx);
        
        updateProgress("Export complete!", totalFrames, totalFrames, 0.0);
        {
            std::lock_guard<std::mutex> lock(progressMutex);
            currentProgress.isComplete = true;
        }
        
        LOG_INFO("Video export completed successfully: " + settings.outputPath);
        return true;
    }
    
    void cancelExport() {
        shouldCancel = true;
        LOG_INFO("Export cancellation requested");
    }
    
    RenderProgress getProgress() const {
        std::lock_guard<std::mutex> lock(progressMutex);
        return currentProgress;
    }
    
private:
    void updateProgress(const std::string& operation, int frame, int total, double remaining) {
        std::lock_guard<std::mutex> lock(progressMutex);
        currentProgress.currentOperation = operation;
        currentProgress.currentFrame = frame;
        currentProgress.totalFrames = total;
        currentProgress.percentage = total > 0 ? (static_cast<double>(frame) / total) * 100.0 : 0.0;
        currentProgress.estimatedTimeRemaining = remaining;
        
        if (progressCallback) {
            progressCallback(currentProgress);
        }
    }
    
    void setError(const std::string& error) {
        std::lock_guard<std::mutex> lock(progressMutex);
        currentProgress.hasError = true;
        currentProgress.errorMessage = error;
        LOG_ERROR(error);
    }
    
    float getParam(const std::unordered_map<std::string, float>& params, 
                   const std::string& key, float defaultValue) {
        auto it = params.find(key);
        return it != params.end() ? it->second : defaultValue;
    }
    
    AVCodecContext* setupVideoEncoder(AVStream* stream, const ExportSettings& settings) {
        const AVCodec* codec = avcodec_find_encoder_by_name(settings.videoCodec.c_str());
        if (!codec) {
            LOG_ERROR("Video codec not found: " + settings.videoCodec);
            return nullptr;
        }
        
        AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
        if (!codecCtx) {
            LOG_ERROR("Could not allocate video codec context");
            return nullptr;
        }
        
        codecCtx->codec_id = codec->id;
        codecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
        codecCtx->width = settings.width;
        codecCtx->height = settings.height;
        codecCtx->time_base = av_d2q(1.0 / settings.frameRate, 1000000);
        codecCtx->framerate = av_d2q(settings.frameRate, 1000000);
        codecCtx->bit_rate = settings.videoBitrate;
        codecCtx->gop_size = static_cast<int>(settings.frameRate); // 1 second GOP
        codecCtx->max_b_frames = 2;
        
        // Set pixel format
        if (settings.pixelFormat == "yuv420p") {
            codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
        } else if (settings.pixelFormat == "yuv444p") {
            codecCtx->pix_fmt = AV_PIX_FMT_YUV444P;
        } else {
            codecCtx->pix_fmt = AV_PIX_FMT_YUV420P; // Default
        }
        
        // Codec specific settings
        if (settings.videoCodec == "libx264") {
            av_opt_set(codecCtx->priv_data, "preset", settings.preset.c_str(), 0);
            av_opt_set(codecCtx->priv_data, "crf", std::to_string(settings.crf).c_str(), 0);
            av_opt_set(codecCtx->priv_data, "profile", "high", 0);
        } else if (settings.videoCodec == "libx265") {
            av_opt_set(codecCtx->priv_data, "preset", settings.preset.c_str(), 0);
            av_opt_set(codecCtx->priv_data, "crf", std::to_string(settings.crf).c_str(), 0);
        }
        
        // Hardware acceleration
        if (settings.hardwareAcceleration && settings.videoCodec.find("nvenc") != std::string::npos) {
            // NVIDIA NVENC settings
            av_opt_set(codecCtx->priv_data, "preset", "slow", 0);
            av_opt_set(codecCtx->priv_data, "rc", "vbr", 0);
        }
        
        stream->time_base = codecCtx->time_base;
        
        int ret = avcodec_open2(codecCtx, codec, nullptr);
        if (ret < 0) {
            LOG_ERROR("Could not open video codec");
            avcodec_free_context(&codecCtx);
            return nullptr;
        }
        
        // Copy parameters to stream
        ret = avcodec_parameters_from_context(stream->codecpar, codecCtx);
        if (ret < 0) {
            LOG_ERROR("Could not copy video codec parameters");
            avcodec_free_context(&codecCtx);
            return nullptr;
        }
        
        return codecCtx;
    }
    
    AVCodecContext* setupAudioEncoder(AVStream* stream, const ExportSettings& settings) {
        const AVCodec* codec = avcodec_find_encoder_by_name(settings.audioCodec.c_str());
        if (!codec) {
            LOG_ERROR("Audio codec not found: " + settings.audioCodec);
            return nullptr;
        }
        
        AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
        if (!codecCtx) {
            LOG_ERROR("Could not allocate audio codec context");
            return nullptr;
        }
        
        codecCtx->codec_id = codec->id;
        codecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
        codecCtx->bit_rate = settings.audioBitrate;
        codecCtx->sample_rate = settings.audioSampleRate;
        codecCtx->channels = 2; // Stereo
        codecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
        codecCtx->time_base = {1, settings.audioSampleRate};
        
        // Set sample format based on codec
        if (codec->sample_fmts) {
            codecCtx->sample_fmt = codec->sample_fmts[0]; // Use first supported format
        } else {
            codecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP; // Default to float planar
        }
        
        stream->time_base = codecCtx->time_base;
        
        int ret = avcodec_open2(codecCtx, codec, nullptr);
        if (ret < 0) {
            LOG_ERROR("Could not open audio codec");
            avcodec_free_context(&codecCtx);
            return nullptr;
        }
        
        // Copy parameters to stream
        ret = avcodec_parameters_from_context(stream->codecpar, codecCtx);
        if (ret < 0) {
            LOG_ERROR("Could not copy audio codec parameters");
            avcodec_free_context(&codecCtx);
            return nullptr;
        }
        
        return codecCtx;
    }
    
    cv::Mat renderVideoFrame(const Timeline& timeline, double currentTime, int width, int height) {
        cv::Mat compositeFrame = cv::Mat::zeros(height, width, CV_8UC3);
        
        // Sort clips by track index for proper layering
        auto sortedClips = timeline.videoTracks;
        std::sort(sortedClips.begin(), sortedClips.end(),
            [](const std::shared_ptr<VideoClip>& a, const std::shared_ptr<VideoClip>& b) {
                return a->trackIndex < b->trackIndex;
            });
        
        // Process each video track
        for (const auto& clip : sortedClips) {
            if (!clip->enabled) continue;
            
            if (currentTime >= clip->startTime && 
                currentTime < clip->startTime + clip->duration) {
                
                double clipTime = currentTime - clip->startTime + clip->inPoint;
                
                // Load frame from video file
                VideoEngine engine;
                if (engine.loadVideo(clip->filePath)) {
                    cv::Mat frame = engine.getFrameAt(clipTime);
                    
                    if (!frame.empty()) {
                        // Resize frame to timeline dimensions
                        cv::resize(frame, frame, cv::Size(width, height));
                        
                        // Apply effects
                        for (const auto& effectName : clip->effects) {
                            frame = applyVideoEffect(frame, effectName, clip->properties);
                        }
                        
                        // Apply opacity and composite
                        if (clip->opacity < 1.0f) {
                            cv::Mat temp;
                            cv::addWeighted(compositeFrame, 1.0f - clip->opacity, frame, clip->opacity, 0, temp);
                            compositeFrame = temp;
                        } else {
                            frame.copyTo(compositeFrame);
                        }
                    }
                }
            }
        }
        
        return compositeFrame;
    }
    
    std::vector<float> renderAudioSamples(const Timeline& timeline, double currentTime, 
                                        double duration, int sampleRate) {
        int sampleCount = static_cast<int>(duration * sampleRate);
        std::vector<float> mixedAudio(sampleCount * 2, 0.0f); // Stereo
        
        // Process each audio track
        for (const auto& clip : timeline.audioTracks) {
            if (!clip->enabled || clip->muted) continue;
            
            if (currentTime >= clip->startTime && 
                currentTime < clip->startTime + clip->duration) {
                
                double clipTime = currentTime - clip->startTime;
                int startSample = static_cast<int>(clipTime * sampleRate);
                
                // Extract samples from waveform (assuming mono input)
                for (int i = 0; i < sampleCount && startSample + i < clip->waveform.size(); i++) {
                    float sample = clip->waveform[startSample + i] * clip->volume;
                    
                    // Convert mono to stereo
                    mixedAudio[i * 2] += sample;     // Left channel
                    mixedAudio[i * 2 + 1] += sample; // Right channel
                }
            }
        }
        
        // Normalize to prevent clipping
        float maxSample = 0.0f;
        for (float sample : mixedAudio) {
            maxSample = std::max(maxSample, std::abs(sample));
        }
        
        if (maxSample > 1.0f) {
            for (float& sample : mixedAudio) {
                sample /= maxSample;
            }
        }
        
        return mixedAudio;
    }
    
    cv::Mat applyVideoEffect(const cv::Mat& frame, const std::string& effectName, 
                           const std::unordered_map<std::string, float>& params) {
        if (effectName == "color_correction") {
            float brightness = getParam(params, "brightness", 0.0f);
            float contrast = getParam(params, "contrast", 1.0f);
            float saturation = getParam(params, "saturation", 1.0f);
            float hue = getParam(params, "hue", 0.0f);
            return EffectProcessor::applyColorCorrection(frame, brightness, contrast, saturation, hue);
        } else if (effectName == "blur") {
            float strength = getParam(params, "strength", 5.0f);
            std::string type = "gaussian"; // Could be extracted from string params
            return EffectProcessor::applyBlur(frame, strength, type);
        } else if (effectName == "glow") {
            float intensity = getParam(params, "intensity", 0.5f);
            float radius = getParam(params, "radius", 10.0f);
            return EffectProcessor::applyGlow(frame, intensity, radius);
        } else if (effectName == "vignette") {
            float strength = getParam(params, "strength", 0.5f);
            return EffectProcessor::applyVignette(frame, strength);
        }
        
        return frame; // No effect applied
    }
    
    bool writeVideoFrame(AVFormatContext* formatCtx, AVCodecContext* codecCtx, 
                        AVStream* stream, const cv::Mat& frame, int frameNumber) {
        AVFrame* avFrame = av_frame_alloc();
        if (!avFrame) return false;
        
        avFrame->format = codecCtx->pix_fmt;
        avFrame->width = codecCtx->width;
        avFrame->height = codecCtx->height;
        avFrame->pts = frameNumber;
        
        int ret = av_frame_get_buffer(avFrame, 0);
        if (ret < 0) {
            av_frame_free(&avFrame);
            return false;
        }
        
        // Convert OpenCV Mat to AVFrame
        SwsContext* swsCtx = sws_getContext(
            frame.cols, frame.rows, AV_PIX_FMT_BGR24,
            codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );
        
        const uint8_t* srcData[4] = {frame.data, nullptr, nullptr, nullptr};
        int srcLinesize[4] = {static_cast<int>(frame.step[0]), 0, 0, 0};
        
        sws_scale(swsCtx, srcData, srcLinesize, 0, frame.rows, avFrame->data, avFrame->linesize);
        sws_freeContext(swsCtx);
        
        // Encode frame
        ret = avcodec_send_frame(codecCtx, avFrame);
        av_frame_free(&avFrame);
        
        if (ret < 0) return false;
        
        AVPacket* packet = av_packet_alloc();
        while (ret >= 0) {
            ret = avcodec_receive_packet(codecCtx, packet);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret < 0) {
                av_packet_free(&packet);
                return false;
            }
            
            packet->stream_index = stream->index;
            av_packet_rescale_ts(packet, codecCtx->time_base, stream->time_base);
            
            ret = av_interleaved_write_frame(formatCtx, packet);
            av_packet_unref(packet);
        }
        
        av_packet_free(&packet);
        return true;
    }
    
    bool writeAudioSamples(AVFormatContext* formatCtx, AVCodecContext* codecCtx,
                          AVStream* stream, const std::vector<float>& samples, int frameNumber) {
        AVFrame* avFrame = av_frame_alloc();
        if (!avFrame) return false;
        
        avFrame->format = codecCtx->sample_fmt;
        avFrame->channels = codecCtx->channels;
        avFrame->sample_rate = codecCtx->sample_rate;
        avFrame->nb_samples = samples.size() / codecCtx->channels;
        avFrame->pts = frameNumber * avFrame->nb_samples;
        
        int ret = av_frame_get_buffer(avFrame, 0);
        if (ret < 0) {
            av_frame_free(&avFrame);
            return false;
        }
        
        // Copy audio samples to frame
        if (codecCtx->sample_fmt == AV_SAMPLE_FMT_FLTP) {
            float* left = (float*)avFrame->data[0];
            float* right = (float*)avFrame->data[1];
            
            for (int i = 0; i < avFrame->nb_samples; i++) {
                left[i] = samples[i * 2];
                right[i] = samples[i * 2 + 1];
            }
        } else {
            // Handle other sample formats as needed
            memcpy(avFrame->data[0], samples.data(), samples.size() * sizeof(float));
        }
        
        // Encode frame
        ret = avcodec_send_frame(codecCtx, avFrame);
        av_frame_free(&avFrame);
        
        if (ret < 0) return false;
        
        AVPacket* packet = av_packet_alloc();
        while (ret >= 0) {
            ret = avcodec_receive_packet(codecCtx, packet);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret < 0) {
                av_packet_free(&packet);
                return false;
            }
            
            packet->stream_index = stream->index;
            av_packet_rescale_ts(packet, codecCtx->time_base, stream->time_base);
            
            ret = av_interleaved_write_frame(formatCtx, packet);
            av_packet_unref(packet);
        }
        
        av_packet_free(&packet);
        return true;
    }
    
    void flushEncoder(AVFormatContext* formatCtx, AVCodecContext* codecCtx, AVStream* stream) {
        avcodec_send_frame(codecCtx, nullptr); // Flush
        
        AVPacket* packet = av_packet_alloc();
        int ret;
        while ((ret = avcodec_receive_packet(codecCtx, packet)) >= 0) {
            packet->stream_index = stream->index;
            av_packet_rescale_ts(packet, codecCtx->time_base, stream->time_base);
            av_interleaved_write_frame(formatCtx, packet);
            av_packet_unref(packet);
        }
        av_packet_free(&packet);
    }
    
    void cleanup(AVFormatContext* formatCtx, AVCodecContext* videoCtx, AVCodecContext* audioCtx) {
        if (videoCtx) avcodec_free_context(&videoCtx);
        if (audioCtx) avcodec_free_context(&audioCtx);
        if (formatCtx) {
            if (!(formatCtx->oformat->flags & AVFMT_NOFILE))
                avio_closep(&formatCtx->pb);
            avformat_free_context(formatCtx);
        }
    }
};

// SI Model for Video Generation and Effects
class SyntheticIntelligence {
public:
    SyntheticIntelligence() {
        LOG_INFO("SyntheticIntelligence initialized");
    }

    // Generate a video frame procedurally
    cv::Mat generateFrame(int width, int height, double time) {
        cv::Mat frame(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

        // Example: Procedural pattern using sine waves
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                uchar value = static_cast<uchar>(
                    (std::sin(x * 0.1 + time) + std::cos(y * 0.1 + time)) * 127 + 128);
                frame.at<cv::Vec3b>(y, x) = cv::Vec3b(value, value, value);
            }
        }

        return frame;
    }

    // Apply effects to a video frame
    cv::Mat applyEffect(const cv::Mat& frame, const std::string& effect, const std::unordered_map<std::string, float>& params) {
        cv::Mat result = frame.clone();

        if (effect == "brightness") {
            float brightness = params.at("value");
            result += cv::Scalar(brightness, brightness, brightness);
        } else if (effect == "contrast") {
            float contrast = params.at("value");
            result.convertTo(result, -1, contrast, 0);
        } else if (effect == "blur") {
            int kernelSize = static_cast<int>(params.at("kernelSize"));
            cv::GaussianBlur(result, result, cv::Size(kernelSize, kernelSize), 0);
        }

        return result;
    }
};

// WebSocket server for frontend communication
class WebSocketServer {
private:
    websocketpp::server<websocketpp::config::asio> server;
    std::thread serverThread;
    std::atomic<bool> running;
    std::mutex clientsMutex;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> clients;
    
    ProjectManager* projectManager;
    RenderEngine* renderEngine;
    VideoEngine* videoEngine;
    AudioEngine* audioEngine;
    
public:
    WebSocketServer(int port = 9002) : running(false), projectManager(nullptr), 
                                      renderEngine(nullptr), videoEngine(nullptr), audioEngine(nullptr) {
        server.set_access_channels(websocketpp::log::alevel::all);
        server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        server.init_asio();
        server.set_reuse_addr(true);
        
        server.set_message_handler([this](websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
            handleMessage(hdl, msg);
        });
        
        server.set_open_handler([this](websocketpp::connection_hdl hdl) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.insert(hdl);
            LOG_INFO("Client connected. Total clients: " + std::to_string(clients.size()));
        });
        
        server.set_close_handler([this](websocketpp::connection_hdl hdl) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.erase(hdl);
            LOG_INFO("Client disconnected. Total clients: " + std::to_string(clients.size()));
        });
        
        server.listen(port);
        server.start_accept();
        
        LOG_INFO("WebSocket server configured on port " + std::to_string(port));
    }
    
    ~WebSocketServer() {
        stop();
    }
    
    void setProjectManager(ProjectManager* pm) { projectManager = pm; }
    void setRenderEngine(RenderEngine* re) { renderEngine = re; }
    void setVideoEngine(VideoEngine* ve) { videoEngine = ve; }
    void setAudioEngine(AudioEngine* ae) { audioEngine = ae; }
    
    bool start() {
        try {
            running = true;
            serverThread = std::thread([this]() {
                server.run();
            });
            LOG_INFO("WebSocket server started successfully");
            return true;
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to start WebSocket server: " + std::string(e.what()));
            return false;
        }
    }
    
    void stop() {
        if (running) {
            running = false;
            server.stop();
            if (serverThread.joinable()) {
                serverThread.join();
            }
            LOG_INFO("WebSocket server stopped");
        }
    }
    
    void broadcast(const Json::Value& message) {
        Json::StreamWriterBuilder builder;
        std::string messageStr = Json::writeString(builder, message);
        
        std::lock_guard<std::mutex> lock(clientsMutex);
        for (auto hdl : clients) {
            try {
                server.send(hdl, messageStr, websocketpp::frame::opcode::text);
            } catch (const std::exception& e) {
                LOG_WARNING("Failed to send message to client: " + std::string(e.what()));
            }
        }
    }
    
private:
    void handleMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
        try {
            std::string payload = msg->get_payload();
            Json::CharReaderBuilder builder;
            Json::Value request;
            std::string errors;
            
            std::istringstream stream(payload);
            if (!Json::parseFromStream(builder, stream, &request, &errors)) {
                sendError(hdl, "Invalid JSON: " + errors);
                return;
            }
            
            std::string command = request.get("command", "").asString();
            Json::Value response;
            response["id"] = request.get("id", "");
            response["command"] = command;
            
            if (command == "ping") {
                response["status"] = "success";
                response["data"] = "pong";
            }
            else if (command == "create_project") {
                handleCreateProject(request, response);
            }
            else if (command == "load_project") {
                handleLoadProject(request, response);
            }
            else if (command == "save_project") {
                handleSaveProject(request, response);
            }
            else if (command == "add_video_clip") {
                handleAddVideoClip(request, response);
            }
            else if (command == "add_audio_clip") {
                handleAddAudioClip(request, response);
            }
            else if (command == "remove_clip") {
                handleRemoveClip(request, response);
            }
            else if (command == "update_clip") {
                handleUpdateClip(request, response);
            }
            else if (command == "get_timeline") {
                handleGetTimeline(request, response);
            }
            else if (command == "export_video") {
                handleExportVideo(request, response);
            }
            else if (command == "cancel_export") {
                handleCancelExport(request, response);
            }
            else if (command == "get_export_progress") {
                handleGetExportProgress(request, response);
            }
            else if (command == "generate_thumbnail") {
                handleGenerateThumbnail(request, response);
            }
            else if (command == "analyze_audio") {
                handleAnalyzeAudio(request, response);
            }
            else if (command == "apply_effect") {
                handleApplyEffect(request, response);
            }
            else if (command == "generate_video") {
                handleGenerateVideo(request, response);
            }
            else if (command == "get_project_info") {
                handleGetProjectInfo(request, response);
            }
            else {
                response["status"] = "error";
                response["error"] = "Unknown command: " + command;
            }
            
            sendResponse(hdl, response);
            
        } catch (const std::exception& e) {
            LOG_ERROR("Error handling WebSocket message: " + std::string(e.what()));
            sendError(hdl, "Internal server error");
        }
    }
    
    void handleCreateProject(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        std::string name = request["params"].get("name", "Untitled").asString();
        int width = request["params"].get("width", 1920).asInt();
        int height = request["params"].get("height", 1080).asInt();
        double fps = request["params"].get("frameRate", 30.0).asDouble();
        
        if (projectManager->createNewProject(name, width, height, fps)) {
            response["status"] = "success";
            response["data"] = projectManager->getProjectInfo();
        } else {
            response["status"] = "error";
            response["error"] = "Failed to create project";
        }
    }
    
    void handleLoadProject(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        std::string filePath = request["params"].get("filePath", "").asString();
        if (filePath.empty()) {
            response["status"] = "error";
            response["error"] = "File path is required";
            return;
        }
        
        if (projectManager->loadProject(filePath)) {
            response["status"] = "success";
            response["data"] = projectManager->getProjectInfo();
        } else {
            response["status"] = "error";
            response["error"] = "Failed to load project";
        }
    }
    
    void handleSaveProject(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        std::string filePath = request["params"].get("filePath", "").asString();
        if (filePath.empty()) {
            response["status"] = "error";
            response["error"] = "File path is required";
            return;
        }
        
        if (projectManager->saveProject(filePath)) {
            response["status"] = "success";
            response["data"]["saved"] = true;
        } else {
            response["status"] = "error";
            response["error"] = "Failed to save project";
        }
    }
    
    void handleAddVideoClip(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        std::string filePath = request["params"].get("filePath", "").asString();
        double startTime = request["params"].get("startTime", 0.0).asDouble();
        int trackIndex = request["params"].get("trackIndex", 0).asInt();
        
        if (filePath.empty()) {
            response["status"] = "error";
            response["error"] = "File path is required";
            return;
        }
        
        std::string clipId = projectManager->addVideoClip(filePath, startTime, trackIndex);
        if (!clipId.empty()) {
            response["status"] = "success";
            response["data"]["clipId"] = clipId;
        } else {
            response["status"] = "error";
            response["error"] = "Failed to add video clip";
        }
    }
    
    void handleAddAudioClip(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        std::string filePath = request["params"].get("filePath", "").asString();
        double startTime = request["params"].get("startTime", 0.0).asDouble();
        int trackIndex = request["params"].get("trackIndex", 0).asInt();
        
        if (filePath.empty()) {
            response["status"] = "error";
            response["error"] = "File path is required";
            return;
        }
        
        std::string clipId = projectManager->addAudioClip(filePath, startTime, trackIndex);
        if (!clipId.empty()) {
            response["status"] = "success";
            response["data"]["clipId"] = clipId;
        } else {
            response["status"] = "error";
            response["error"] = "Failed to add audio clip";
        }
    }
    
    void handleRemoveClip(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        std::string clipId = request["params"].get("clipId", "").asString();
        if (clipId.empty()) {
            response["status"] = "error";
            response["error"] = "Clip ID is required";
            return;
        }
        
        if (projectManager->removeClip(clipId)) {
            response["status"] = "success";
            response["data"]["removed"] = true;
        } else {
            response["status"] = "error";
            response["error"] = "Failed to remove clip";
        }
    }
    
    void handleUpdateClip(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        std::string clipId = request["params"].get("clipId", "").asString();
        const Json::Value& updates = request["params"]["updates"];
        
        if (clipId.empty()) {
            response["status"] = "error";
            response["error"] = "Clip ID is required";
            return;
        }
        
        if (projectManager->updateClip(clipId, updates)) {
            response["status"] = "success";
            response["data"]["updated"] = true;
        } else {
            response["status"] = "error";
            response["error"] = "Failed to update clip";
        }
    }
    
    void handleGetTimeline(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        Timeline& timeline = projectManager->getTimeline();
        Json::Value timelineData;
        
        timelineData["name"] = timeline.name;
        timelineData["duration"] = timeline.duration;
        timelineData["width"] = timeline.width;
        timelineData["height"] = timeline.height;
        timelineData["frameRate"] = timeline.frameRate;
        
        // Serialize video tracks
        Json::Value videoTracks(Json::arrayValue);
        for (const auto& clip : timeline.videoTracks) {
            Json::Value clipData;
            clipData["id"] = clip->id;
            clipData["filePath"] = clip->filePath;
            clipData["startTime"] = clip->startTime;
            clipData["duration"] = clip->duration;
            clipData["inPoint"] = clip->inPoint;
            clipData["outPoint"] = clip->outPoint;
            clipData["trackIndex"] = clip->trackIndex;
            clipData["enabled"] = clip->enabled;
            clipData["opacity"] = clip->opacity;
            
            videoTracks.append(clipData);
        }
        timelineData["videoTracks"] = videoTracks;
        
        // Serialize audio tracks
        Json::Value audioTracks(Json::arrayValue);
        for (const auto& clip : timeline.audioTracks) {
            Json::Value clipData;
            clipData["id"] = clip->id;
            clipData["filePath"] = clip->filePath;
            clipData["startTime"] = clip->startTime;
            clipData["duration"] = clip->duration;
            clipData["volume"] = clip->volume;
            clipData["trackIndex"] = clip->trackIndex;
            clipData["enabled"] = clip->enabled;
            clipData["muted"] = clip->muted;
            
            // Include waveform data (sample every 10th point to reduce size)
            Json::Value waveform(Json::arrayValue);
            for (size_t i = 0; i < clip->waveform.size(); i += 10) {
                waveform.append(clip->waveform[i]);
            }
            clipData["waveform"] = waveform;
            
            audioTracks.append(clipData);
        }
        timelineData["audioTracks"] = audioTracks;
        
        response["status"] = "success";
        response["data"] = timelineData;
    }
    
    void handleExportVideo(const Json::Value& request, Json::Value& response) {
        if (!renderEngine || !projectManager) {
            response["status"] = "error";
            response["error"] = "Render engine or project manager not available";
            return;
        }
        
        RenderEngine::ExportSettings settings;
        const Json::Value& params = request["params"];
        
        settings.outputPath = params.get("outputPath", "output.mp4").asString();
        settings.videoCodec = params.get("videoCodec", "libx264").asString();
        settings.audioCodec = params.get("audioCodec", "aac").asString();
        settings.width = params.get("width", 1920).asInt();
        settings.height = params.get("height", 1080).asInt();
        settings.frameRate = params.get("frameRate", 30.0).asDouble();
        settings.videoBitrate = params.get("videoBitrate", 10000000).asInt();
        settings.audioBitrate = params.get("audioBitrate", 192000).asInt();
        settings.preset = params.get("preset", "medium").asString();
        settings.crf = params.get("crf", 23).asInt();
        
        // Start export in a separate thread
        std::thread exportThread([this, settings]() {
            Timeline& timeline = projectManager->getTimeline();
            bool success = renderEngine->exportVideo(timeline, settings);
            
            // Broadcast completion status
            Json::Value notification;
            notification["type"] = "export_complete";
            notification["success"] = success;
            notification["outputPath"] = settings.outputPath;
            broadcast(notification);
        });
        exportThread.detach();
        
        response["status"] = "success";
        response["data"]["exportStarted"] = true;
    }
    
    void handleCancelExport(const Json::Value& request, Json::Value& response) {
        if (!renderEngine) {
            response["status"] = "error";
            response["error"] = "Render engine not available";
            return;
        }
        
        renderEngine->cancelExport();
        response["status"] = "success";
        response["data"]["cancelled"] = true;
    }
    
    void handleGetExportProgress(const Json::Value& request, Json::Value& response) {
        if (!renderEngine) {
            response["status"] = "error";
            response["error"] = "Render engine not available";
            return;
        }
        
        RenderEngine::RenderProgress progress = renderEngine->getProgress();
        
        Json::Value progressData;
        progressData["currentFrame"] = progress.currentFrame;
        progressData["totalFrames"] = progress.totalFrames;
        progressData["percentage"] = progress.percentage;
        progressData["estimatedTimeRemaining"] = progress.estimatedTimeRemaining;
        progressData["currentOperation"] = progress.currentOperation;
        progressData["isComplete"] = progress.isComplete;
        progressData["hasError"] = progress.hasError;
        progressData["errorMessage"] = progress.errorMessage;
        
        response["status"] = "success";
        response["data"] = progressData;
    }
    
    void handleGenerateThumbnail(const Json::Value& request, Json::Value& response) {
        if (!videoEngine) {
            response["status"] = "error";
            response["error"] = "Video engine not available";
            return;
        }
        
        std::string filePath = request["params"].get("filePath", "").asString();
        double timeSeconds = request["params"].get("timeSeconds", 5.0).asDouble();
        int width = request["params"].get("width", 160).asInt();
        int height = request["params"].get("height", 90).asInt();
        
        if (filePath.empty()) {
            response["status"] = "error";
            response["error"] = "File path is required";
            return;
        }
        
        cv::Mat thumbnail = videoEngine->generateThumbnail(filePath, timeSeconds, cv::Size(width, height));
        
        if (!thumbnail.empty()) {
            // Convert to base64 for transmission
            std::vector<uchar> buffer;
            cv::imencode(".jpg", thumbnail, buffer);
            std::string encoded = base64_encode(buffer.data(), buffer.size());
            
            response["status"] = "success";
            response["data"]["thumbnail"] = "data:image/jpeg;base64," + encoded;
        } else {
            response["status"] = "error";
            response["error"] = "Failed to generate thumbnail";
        }
    }
    
    void handleAnalyzeAudio(const Json::Value& request, Json::Value& response) {
        if (!audioEngine) {
            response["status"] = "error";
            response["error"] = "Audio engine not available";
            return;
        }
        
        std::string filePath = request["params"].get("filePath", "").asString();
        std::string analysisType = request["params"].get("type", "waveform").asString();
        
        if (filePath.empty()) {
            response["status"] = "error";
            response["error"] = "File path is required";
            return;
        }
        
        Json::Value analysisData;
        
        if (analysisType == "waveform") {
            int sampleCount = request["params"].get("sampleCount", 1000).asInt();
            std::vector<float> waveform = audioEngine->generateWaveform(filePath, sampleCount);
            
            Json::Value waveformArray(Json::arrayValue);
            for (float sample : waveform) {
                waveformArray.append(sample);
            }
            analysisData["waveform"] = waveformArray;
            
        } else if (analysisType == "beats") {
            std::vector<float> audioData = audioEngine->generateWaveform(filePath, -1); // Full resolution
            std::vector<float> beats = audioEngine->detectBeats(audioData);
            
            Json::Value beatsArray(Json::arrayValue);
            for (float beatTime : beats) {
                beatsArray.append(beatTime);
            }
            analysisData["beats"] = beatsArray;
            
        } else if (analysisType == "spectrum") {
            // Simplified spectrum analysis
            std::vector<float> audioData = audioEngine->generateWaveform(filePath, -1);
            if (!audioData.empty()) {
                std::vector<float> spectrum = audioEngine->analyzeFrequencySpectrum(audioData, 0);
                
                Json::Value spectrumArray(Json::arrayValue);
                for (float magnitude : spectrum) {
                    spectrumArray.append(magnitude);
                }
                analysisData["spectrum"] = spectrumArray;
            }
        }
        
        analysisData["duration"] = audioEngine->getAudioDuration(filePath);
        
        response["status"] = "success";
        response["data"] = analysisData;
    }
    
    void handleApplyEffect(const Json::Value& request, Json::Value& response) {
        // This would be implemented based on specific effect requirements
        // For now, just acknowledge the request
        response["status"] = "success";
        response["data"]["effectApplied"] = true;
    }
    
    void handleGetProjectInfo(const Json::Value& request, Json::Value& response) {
        if (!projectManager) {
            response["status"] = "error";
            response["error"] = "Project manager not available";
            return;
        }
        
        response["status"] = "success";
        response["data"] = projectManager->getProjectInfo();
    }
    
    void handleCreateAIVideo(const Json::Value& request, Json::Value& response) {
        if (!videoEngine) {
            response["status"] = "error";
            response["error"] = "Video engine not available";
            return;
        }
        
        std::string prompt = request["params"].get("prompt", "").asString();
        int width = request["params"].get("width", 1920).asInt();
        int height = request["params"].get("height", 1080).asInt();
        
        if (prompt.empty()) {
            response["status"] = "error";
            response["error"] = "Prompt is required";
            return;
        }
        
        VideoCreationAI aiModel("/path/to/model");
        cv::Mat frame = aiModel.generateVideoFrame(prompt, width, height);
        
        if (!frame.empty()) {
            // Save the frame as a video clip or return it as a response
            std::string outputPath = "ai_generated_frame.jpg";
            cv::imwrite(outputPath, frame);
            
            response["status"] = "success";
            response["data"]["outputPath"] = outputPath;
        } else {
            response["status"] = "error";
            response["error"] = "Failed to generate video frame";
        }
    }
    
    void handleGenerateVideo(const Json::Value& request, Json::Value& response) {
        int width = request["params"].get("width", 1920).asInt();
        int height = request["params"].get("height", 1080).asInt();
        double duration = request["params"].get("duration", 5.0).asDouble();
        double frameRate = request["params"].get("frameRate", 30.0).asDouble();

        SyntheticIntelligence si;
        std::string outputPath = "generated_video.avi";
        cv::VideoWriter writer(outputPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), frameRate, cv::Size(width, height));

        if (!writer.isOpened()) {
            response["status"] = "error";
            response["error"] = "Failed to open video writer";
            return;
        }

        for (int frameNumber = 0; frameNumber < duration * frameRate; ++frameNumber) {
            double time = frameNumber / frameRate;
            cv::Mat frame = si.generateFrame(width, height, time);
            writer.write(frame);
        }

        writer.release();

        response["status"] = "success";
        response["data"]["outputPath"] = outputPath;
    }
    
    void handleApplyEffect(const Json::Value& request, Json::Value& response) {
        std::string inputPath = request["params"].get("inputPath", "").asString();
        std::string outputPath = request["params"].get("outputPath", "output_effect.avi").asString();
        std::string effect = request["params"].get("effect", "").asString();

        if (inputPath.empty() || effect.empty()) {
            response["status"] = "error";
            response["error"] = "Input path and effect are required";
            return;
        }

        cv::VideoCapture capture(inputPath);
        if (!capture.isOpened()) {
            response["status"] = "error";
            response["error"] = "Failed to open input video";
            return;
        }

        int width = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
        int height = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
        double frameRate = capture.get(cv::CAP_PROP_FPS);

        cv::VideoWriter writer(outputPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), frameRate, cv::Size(width, height));
        if (!writer.isOpened()) {
            response["status"] = "error";
            response["error"] = "Failed to open video writer";
            return;
        }

        SyntheticIntelligence si;
        cv::Mat frame;
        while (capture.read(frame)) {
            std::unordered_map<std::string, float> params;
            for (const auto& key : request["params"]["effectParams"].getMemberNames()) {
                params[key] = request["params"]["effectParams"].get(key, 0.0).asFloat();
            }

            cv::Mat processedFrame = si.applyEffect(frame, effect, params);
            writer.write(processedFrame);
        }

        capture.release();
        writer.release();

        response["status"] = "success";
        response["data"]["outputPath"] = outputPath;
    }
    
    void sendResponse(websocketpp::connection_hdl hdl, const Json::Value& response) {
        Json::StreamWriterBuilder builder;
        std::string responseStr = Json::writeString(builder, response);
        
        try {
            server.send(hdl, responseStr, websocketpp::frame::opcode::text);
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to send response: " + std::string(e.what()));
        }
    }
    
    void sendError(websocketpp::connection_hdl hdl, const std::string& error) {
        Json::Value response;
        response["status"] = "error";
        response["error"] = error;
        sendResponse(hdl, response);
    }
    
    // Base64 encoding utility
    std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        
        const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        
        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                
                for(i = 0; (i <4) ; i++)
                    ret += chars[char_array_4[i]];
                i = 0;
            }
        }
        
        if (i) {
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';
                
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for (j = 0; (j < i + 1); j++)
                ret += chars[char_array_4[j]];
                
            while((i++ < 3))
                ret += '=';
        }
        
        return ret;
    }
};

// AI Model Integration for Video Creation
class VideoCreationAI {
private:
    std::string modelPath;
    std::mutex aiMutex;

public:
    VideoCreationAI(const std::string& path) : modelPath(path) {
        LOG_INFO("VideoCreationAI initialized with model path: " + modelPath);
    }

    cv::Mat generateVideoFrame(const std::string& prompt, int width, int height) {
        std::lock_guard<std::mutex> lock(aiMutex);

        // Placeholder for AI model inference logic
        // Load the model and generate a frame based on the prompt
        cv::Mat frame = cv::Mat::zeros(height, width, CV_8UC3);
        cv::putText(frame, "AI Frame: " + prompt, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);

        return frame;
    }
};

// Performance monitoring system
class PerformanceMonitor {
private:
    std::atomic<size_t> memoryUsage;
    std::atomic<double> cpuUsage;
    std::atomic<double> gpuUsage;
    std::atomic<int> activeThreads;
    std::thread monitorThread;
    std::atomic<bool> running;
    std::mutex statsMutex;
    std::chrono::steady_clock::time_point lastUpdate;
    
public:
    struct SystemStats {
        size_t totalMemory;
        size_t availableMemory;
        size_t usedMemory;
        double memoryPercentage;
        double cpuPercentage;
        double gpuPercentage;
        int processorCount;
        int activeThreadCount;
        double frameRate;
        std::string gpuName;
        size_t gpuMemoryTotal;
        size_t gpuMemoryUsed;
    };
    
    PerformanceMonitor() : memoryUsage(0), cpuUsage(0.0), gpuUsage(0.0), activeThreads(0), running(false) {
        lastUpdate = std::chrono::steady_clock::now();
    }
    
    ~PerformanceMonitor() {
        stop();
    }
    
    void start() {
        if (running) return;
        
        running = true;
        monitorThread = std::thread([this]() {
            monitorLoop();
        });
        
        LOG_INFO("Performance monitor started");
    }
    
    void stop() {
        if (running) {
            running = false;
            if (monitorThread.joinable()) {
                monitorThread.join();
            }
            LOG_INFO("Performance monitor stopped");
        }
    }
    
    SystemStats getSystemStats() {
        std::lock_guard<std::mutex> lock(statsMutex);
        
        SystemStats stats;
        stats.usedMemory = memoryUsage.load();
        stats.cpuPercentage = cpuUsage.load();
        stats.gpuPercentage = gpuUsage.load();
        stats.activeThreadCount = activeThreads.load();
        
        // Get system memory info
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        stats.totalMemory = getTotalSystemMemory();
        stats.availableMemory = getAvailableMemory();
        stats.memoryPercentage = (static_cast<double>(stats.usedMemory) / stats.totalMemory) * 100.0;
        
        stats.processorCount = std::thread::hardware_concurrency();
        stats.frameRate = calculateFrameRate();
        
#ifdef USE_CUDA
        // Get GPU stats if CUDA is available
        size_t free, total;
        if (cudaMemGetInfo(&free, &total) == cudaSuccess) {
            stats.gpuMemoryTotal = total;
            stats.gpuMemoryUsed = total - free;
        }
        
        cudaDeviceProp prop;
        if (cudaGetDeviceProperties(&prop, 0) == cudaSuccess) {
            stats.gpuName = std::string(prop.name);
        }
#endif
        
        return stats;
    }
    
    void updateMemoryUsage(size_t bytes) {
        memoryUsage = bytes;
    }
    
    void incrementActiveThreads() {
        activeThreads++;
    }
    
    void decrementActiveThreads() {
        activeThreads--;
    }
    
private:
    void monitorLoop() {
        while (running) {
            updateStats();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void updateStats() {
        std::lock_guard<std::mutex> lock(statsMutex);
        
        // Update CPU usage
        cpuUsage = getCurrentCPUUsage();
        
        // Update GPU usage
#ifdef USE_CUDA
        gpuUsage = getCurrentGPUUsage();
#endif
        
        lastUpdate = std::chrono::steady_clock::now();
    }
    
    double getCurrentCPUUsage() {
        static clock_t lastCPU = clock();
        static struct timespec lastTime;
        static bool firstCall = true;
        
        if (firstCall) {
            clock_gettime(CLOCK_MONOTONIC, &lastTime);
            firstCall = false;
            return 0.0;
        }
        
        struct timespec currentTime;
        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        
        clock_t currentCPU = clock();
        double cpuTimeDiff = static_cast<double>(currentCPU - lastCPU) / CLOCKS_PER_SEC;
        double realTimeDiff = currentTime.tv_sec - lastTime.tv_sec + (currentTime.tv_nsec - lastTime.tv_nsec) / 1e9;
        
        lastCPU = currentCPU;
        lastTime = currentTime;
        
        return (cpuTimeDiff / realTimeDiff) * 100.0;
    }
    
    double getCurrentGPUUsage() {
        // Placeholder for GPU usage calculation
        return 0.0;
    }
};