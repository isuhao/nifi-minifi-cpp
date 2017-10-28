/**
 * @file FocusArchiveEntry.h
 * FocusArchiveEntry class declaration
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef LIBMINIFI_INCLUDE_PROCESSORS_FOCUSARCHIVEENTRY_H_
#define LIBMINIFI_INCLUDE_PROCESSORS_FOCUSARCHIVEENTRY_H_

#include <list>
#include <memory>
#include <string>

#include <archive.h>

#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "core/Core.h"
#include "core/logging/LoggerConfiguration.h"
#include "core/Resource.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

//! FocusArchiveEntry Class
class FocusArchiveEntry : public core::Processor {
 public:
  //! Constructor
  /*!
   * Create a new processor
   */
  explicit FocusArchiveEntry(std::string name, uuid_t uuid = NULL)
  : core::Processor(name, uuid),
    logger_(logging::LoggerFactory<FocusArchiveEntry>::getLogger()) {
  }
  //! Destructor
  virtual ~FocusArchiveEntry()   {
  }
  //! Processor Name
  static constexpr char const* ProcessorName = "FocusArchiveEntry";
  //! Supported Properties
  static core::Property Path;
  //! Supported Relationships
  static core::Relationship Success;

  bool set_or_update_attr(std::shared_ptr<core::FlowFile>, const std::string&, const std::string&) const;

  //! OnTrigger method, implemented by NiFi FocusArchiveEntry
  virtual void onTrigger(core::ProcessContext *context,
      core::ProcessSession *session);
  //! Initialize, over write by NiFi FocusArchiveEntry
  virtual void initialize(void);

  typedef struct {
    std::string entryName;
    std::string tmpFileName;
    std::string stashKey;
    mode_t entryType;
    mode_t entryPerm;
    uid_t entryUID;
    gid_t entryGID;
    uint64_t entryMTime;
    uint64_t entryMTimeNsec;
    uint64_t entrySize;
  } ArchiveEntryMetadata;

  typedef struct {
    std::string archiveFormatName;
    int archiveFormat;
    std::string focusedEntry;
    std::list<ArchiveEntryMetadata> entryMetadata;
  } ArchiveMetadata;

  class ReadCallback : public InputStreamCallback {
   public:
    explicit ReadCallback(core::Processor*, ArchiveMetadata *archiveMetadata);
    ~ReadCallback();
    virtual int64_t process(std::shared_ptr<io::BaseStream> stream);
    bool isRunning() {return proc_->isRunning();}

   private:
    core::Processor * const proc_;
    std::shared_ptr<logging::Logger> logger_;
    ArchiveMetadata *_archiveMetadata;
    static int ok_cb(struct archive *, void *d) { return ARCHIVE_OK; }
    static ssize_t read_cb(struct archive * a, void *d, const void **buf);
  };

 private:
  //! Logger
  std::shared_ptr<logging::Logger> logger_;
  static std::shared_ptr<utils::IdGenerator> id_generator_;
};

REGISTER_RESOURCE(FocusArchiveEntry);

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif  // LIBMINIFI_INCLUDE_PROCESSORS_FOCUSARCHIVEENTRY_H_