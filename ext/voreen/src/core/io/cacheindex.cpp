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

#include "voreen/core/io/cache.h"

#include "tgt/filesystem.h"
#include "voreen/core/voreenapplication.h"
#include "voreen/core/io/volumeserializer.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/processors/processor.h"

#include <iostream>
#include <queue>
#include <time.h>

using tgt::FileSystem;

namespace voreen {

// Implementations for class CacheIndexSubEntry
//

CacheIndex::CacheIndexEntry::CacheIndexSubEntry::CacheIndexSubEntry(const std::string& processorState,
                                                                    const std::string& processorInportConfig,
                                                                    const std::string& filename)
    : processorState_(processorState),
      processorInportConfig_(processorInportConfig),
      filename_(filename),
      time_(static_cast<unsigned long>(time(0))),
      refCounter_(0)
{
}

CacheIndex::CacheIndexEntry::CacheIndexSubEntry::CacheIndexSubEntry(TiXmlElement* const /*xml*/)
    : processorState_(""),
      processorInportConfig_(""),
      filename_(""),
      time_(0),
      refCounter_(0)
{
}

std::string CacheIndex::CacheIndexEntry::CacheIndexSubEntry::makeKey() const {
    return std::string("InportConfig{" + processorInportConfig_
        + "}.State{" + processorState_ + "}");
}

// ----------------------------------------------------------------------------

// Implementations for class CacheIndexEntry
//

const std::string CacheIndex::CacheIndexEntry::loggerCat_ = "CacheIndexEntry";

CacheIndex::CacheIndexEntry::CacheIndexEntry(const CacheIndex::CacheIndexEntry& cie,
                                             const std::vector<CacheIndexSubEntry>& sub)
    : processorClassName_(cie.processorClassName_),
      processorName_(cie.processorName_),
      portName_(cie.portName_),
      objectClassName_(cie.objectClassName_),
      refCounter_(0),
      sub_()
{
    for (size_t i = 0; i < sub.size(); ++i)
        sub_.insert(std::make_pair(sub[i].makeKey(), sub[i]));
}

CacheIndex::CacheIndexEntry::CacheIndexEntry(const std::string& processorClassName,
                                             const std::string& processorName, const std::string& portName,
                                             const std::string& objectClassName)
    : processorClassName_(processorClassName),
      processorName_(processorName),
      portName_(portName),
      objectClassName_(objectClassName),
      refCounter_(0),
      sub_()
{
}

CacheIndex::CacheIndexEntry::CacheIndexEntry(TiXmlElement* const /*xml*/)
    : processorClassName_(""),
      processorName_(""),
      portName_(""),
      objectClassName_(""),
      refCounter_(0),
      sub_()
{
}

std::string CacheIndex::CacheIndexEntry::findFilename(const std::string& subKey) {
    SubEntryMap::iterator it = sub_.find(subKey);
    if (it == sub_.end())
        return "";
    CacheIndexSubEntry& sub = it->second;
    ++(sub.refCounter_);     // increment reference counter to indicate access to the data
    return sub.filename_;
}

std::vector<CacheIndex::CacheIndexEntry::CacheIndexSubEntry> CacheIndex::CacheIndexEntry::getAllSubEntries() {
    std::vector<CacheIndex::CacheIndexEntry::CacheIndexSubEntry> sub;
    for (SubEntryMap::const_iterator it = sub_.begin(); it != sub_.end(); ++it)
        sub.push_back(it->second);
    return sub;
}

void CacheIndex::CacheIndexEntry::incrementRefCounter(const std::string& subKey) {
    SubEntryMap::iterator it = sub_.find(subKey);
    if (it != sub_.end())
        ++((it->second).refCounter_);
}

bool CacheIndex::CacheIndexEntry::insert(const CacheIndexSubEntry& cid) {
    std::pair<SubEntryMap::iterator, bool> res = sub_.insert(std::make_pair(cid.makeKey(), cid));
    return res.second;
}

std::string CacheIndex::CacheIndexEntry::insert(const std::string& processorState,
                                    const std::string& processorInportConfig,
                                    const std::string& filename)
{
    CacheIndexSubEntry cise(processorState, processorInportConfig, filename);
    std::string subKey = cise.makeKey();

    if (subEntryExists(subKey) == false) {
        freeDataRefCount();
        std::pair<SubEntryMap::iterator, bool> res = sub_.insert(std::make_pair(subKey, cise));
        if (res.second == true)
            return subKey;
        else {
            LDEBUG("insert(): fatal error! Sub-entry already exists in local sub-entry map!");
            return "";
        }
    }
    return subKey;
}

std::string CacheIndex::CacheIndexEntry::makeKey() const {
    return std::string("Processor{" + processorClassName_
        + "}.Outport{" + portName_ + "}");
}

bool CacheIndex::CacheIndexEntry::subEntryExists(const std::string& subKey) const {
    SubEntryMap::const_iterator it = sub_.find(subKey);
    return (it != sub_.end());
}

// private methods
//

size_t CacheIndex::CacheIndexEntry::freeDataRefCount() {
    const size_t CACHE_DATA_LIMIT = 3; // HACK: replace by user-definable threshold
    if (sub_.size() < CACHE_DATA_LIMIT)
        return 0;

    // Sort the subentries by their reference counter (or creation time, if reference
    // counters are equal) by inserting them into a priority queue.
    //
    std::priority_queue<CacheIndexSubEntry*, std::vector<CacheIndexSubEntry*>,
        CacheIndexSubEntry::SubEntryComparator> subEntryQueue;
    for (SubEntryMap::iterator it = sub_.begin(); it != sub_.end(); ++it)
        subEntryQueue.push(&(it->second));

    // Remove subentries until the number of subentries is less than
    // pre-defined limit.
    //
    size_t removed = 0;
    while (subEntryQueue.size() >= CACHE_DATA_LIMIT) {
        CacheIndexSubEntry* sub = subEntryQueue.top();
        subEntryQueue.pop();
        displacedSubEntries_.push_back(*sub);
        sub_.erase(sub->makeKey());
        ++removed;
    }
    return removed;
}

// ----------------------------------------------------------------------------

// Implementations for class CacheIndex
//

const std::string CacheIndex::loggerCat_("CacheIndex");
const std::string CacheIndex::indexFilename_("cacheindex.xml");

CacheIndex::~CacheIndex() {
    /*
    if (CacheBase::isCachingEnabled() == true)
        writeIndexFile(); */
}

std::vector<std::pair<std::string, std::string> > CacheIndex::cleanup() {
    std::vector<std::pair<std::string, std::string> > dumps;

    for (size_t i = 0; i < displacedEntries_.size(); ++i) {
        CacheIndexEntry& entry = displacedEntries_[i];
        std::string entryKey = entry.makeKey();

        std::vector<CacheIndexEntry::CacheIndexSubEntry> subEntry = entry.getAllSubEntries();
        for (size_t j = 0; j < subEntry.size(); ++j) {
            std::string key = entryKey + "." + subEntry[j].makeKey();
            std::string file = CacheIndex::cacheFolder_ + "/" + subEntry[j].getFilename();
            dumps.push_back(std::make_pair(key, file));
        }
        entry.displacedSubEntries_.clear();
    }
    displacedEntries_.clear();
    return dumps;
}

bool CacheIndex::entryExists(const std::string& key) const {
    EntryMap::const_iterator it = entries_.find(key);
    return (it != entries_.end());
}

std::string CacheIndex::findFilename(Processor* const processor, Port* const port,
                                     const std::string& inportConfig)
{
    if ((processor == 0) || (port == 0))
        return "";

    IndexKey key = generateCacheIndexKey(processor, port, inportConfig);
    EntryMap::iterator it = entries_.find(key.first);
    if (it == entries_.end()) {
        LDEBUG("findFilename(): no entry found for key '" << key.first << "'!");
        return "";
    }

    CacheIndex::CacheIndexEntry& entry = it->second;
    ++(entry.refCounter_);  // increase reference counter to indicate access of the entry
    std::string filename = entry.findFilename(key.second);
    if (filename.empty() == true) {
        LDEBUG("findFilename(): no data found for key '" << key.second << "'!");
        return "";
    }
    return std::string(cacheFolder_ + "/" + filename);
}

void CacheIndex::incrementRefCounter(const IndexKey& key) {
    EntryMap::iterator it = entries_.find(key.first);
    if (it != entries_.end()) {
        CacheIndexEntry& entry = it->second;
        ++(entry.refCounter_);
        entry.incrementRefCounter(key.second);
    }
}

std::string CacheIndex::insert(Processor* const processor, Port* const port,
                                           const std::string& objectClassName,
                                           const std::string& /*inportConfig*/,
                                           const std::string& /*filename*/)
{
    CacheIndex::CacheIndexEntry cie(processor->getClassName(), processor->getName(),
        port->getName(), objectClassName);
    std::string entryKey = cie.makeKey();

    // If the key is a new one and needs to be inserted, check whether the threshold for
    // the maximal number of entries has not been reached yet and free entries otherwise
    // by using their reference counters.
    //
    if (entryExists(entryKey) == false)
        freeEntriesRefCount();

    // Insert the newly obtained key into the entry map. If the key causes a new subentry to be
    // created for an exisiting entry, and the limit for the maximum number of subentries has
    // been reached for that particular entry, subentries might become displaced and stored
    // in displacedSubEntries_ of that entry.
    //
    std::pair<EntryMap::iterator, bool> result1 = entries_.insert(std::make_pair(entryKey, cie));
    CacheIndex::CacheIndexEntry& entry = (result1.first)->second;
    std::string subKey; /* = entry.insert(processor->getState(), inportConfig,
        filename); */

    // Take eventually displaced subentries in the current entry, make a copy of that
    // entry without its not-displaced subentries and add the DISPLACED subentries.
    // The copied entry containing its displaced subentries is then stored in
    // displacedEntries_.
    //
    if (entry.displacedSubEntries_.empty() == false) {
        displacedEntries_.push_back(CacheIndex::CacheIndexEntry(entry, entry.displacedSubEntries_));
        entry.displacedSubEntries_.clear();
    }

    // If the entry has been inserted and the index file needs to be written imediately,
    // write the index file. Otherwise the index file will be written on destruction
    // of this CacheIndex object.
    //
    if ((instantWrite_ == true) && (subKey.empty() == false)) {
        //if (writeIndexFile() == false)
            //LERROR("CacheIndex::insert(): failed to write index file!");
    }

    if (subKey.empty() == false)
        return std::string(entry.makeKey() + "." + subKey);

    return "";
}

CacheIndex::IndexKey CacheIndex::generateCacheIndexKey(Processor* const processor,
                                                              Port* const port,
                                                              const std::string& inportConfig)
{
    if ((processor == 0) || (port == 0))
        return IndexKey("", "");

    return IndexKey(std::string("Processor{" + processor->getClassName()
        + "}.Outport{" + port->getName() + "}"),
        std::string ("InportConfig{" + inportConfig
        + "}.State{" + /*processor->getState() +*/ "}"));
}

// private methods
//

CacheIndex::CacheIndex()
    : cacheFolder_(VoreenApplication::app()->getCachePath())
{
    if (CacheBase::isCachingEnabled() == true) {
        if (prepareCacheFolder() == true)
            ;//readIndexFile();
        else {
            LINFO("Failed to prepare the directory used for cached data. The cache will be disabled.");
            LINFO("Please check your rights to access the local file system.");
            CacheBase::setCachingEnabled(false);
        }
    }
}

size_t CacheIndex::freeEntriesRefCount() {
    const size_t CACHE_ENTRIES_LIMIT = 5; // HACK: replace by user-definable threshold
    if (entries_.size() < CACHE_ENTRIES_LIMIT)
        return 0;

    std::priority_queue<CacheIndex::CacheIndexEntry*, std::vector<CacheIndex::CacheIndexEntry*>,
        CacheIndex::CacheIndexEntry::EntryComparator> entriesQueue;
    for (EntryMap::iterator it = entries_.begin(); it != entries_.end(); ++it)
        entriesQueue.push(&(it->second));

    size_t removed = 0;
    while (entriesQueue.size() >= CACHE_ENTRIES_LIMIT) {
        CacheIndex::CacheIndexEntry* entry = entriesQueue.top();
        entriesQueue.pop();
        displacedEntries_.push_back(*entry);
        entries_.erase(entry->makeKey());
        ++removed;
    }
    return removed;
}

bool CacheIndex::prepareCacheFolder() {
    if (FileSystem::dirExists(cacheFolder_))
        return true;
    else {
        LINFO("Cache directory '" << cacheFolder_ << "' seems to not exist. Trying to create it...");
        bool res = FileSystem::createDirectory(cacheFolder_);
        if (res)
            LINFO("Directory '" << cacheFolder_ << "' created successfully!");
        return res;
    }
}

} // namespace voreen
