/**	read, write and access BAM files through bamtools library */

#ifndef _BAMSEQLIBSACCESS_HH
#define _BAMSEQLIBACCESS_HH

#include <iostream>
#include <vector>
#include <SeqLib/BamRecord.h>
#include <SeqLib/BamReader.h>
#include <SeqLib/BamWriter.h>
#include <htslib/hts.h>
#include "bamaccess.hh"
#include "filterBam.h"

#ifdef SEQLIB_1_2
typedef std::shared_ptr<SeqLib::ThreadPool> ThreadPool_;
#endif

class BamSeqLibAlignmentRecord : public BamAlignmentRecord {
private:
    std::shared_ptr<SeqLib::BamRecord> alignment;
    std::shared_ptr<SeqLib::BamHeader> bamHeader;
public:

    /**
     * BamToolsAlignmentRecord constructor
     */
    BamSeqLibAlignmentRecord(std::shared_ptr<SeqLib::BamRecord> alignment, std::shared_ptr<SeqLib::BamHeader> &bamHeader);

    /** returns the wrapped BamRecord
     */
    std::shared_ptr<SeqLib::BamRecord> getAlignment() const;

    /** returns the Query template NAME of the alignment, i.e the QNAME Field
     */
    std::string getQueryName() const override;

    /** returns the length of the query sequence of the alignment, i.e the length of the SEQ Field
     */
    int32_t getQuerySequenceLength() const override;

    /** returns position (0-based) where alignment starts
     */
    int32_t getStartPosition() const override;

    /** returns alignment end position
     */
    int32_t getEndPosition() const override;

    /** returns true if alignment mapped to reverse strand
     */
    bool isReverseStrand() const override;

    /** returns true if alignment part of paired-end read
     */
    bool isPaired() const override;

    /** returns true if alignment is mapped
     */
    bool isMapped() const override;

    /** returns true if alignment's mate is mapped
     */
    bool isMateMapped() const override;

    /** returns true if alignment is first mate on read
     */
    bool isFirstMate() const override;

    /** returns true if alignment is second mate on read
     */
    bool isSecondMate() const override;

    /** returns ID number for reference sequence
     */
    int32_t getRefID() const override;

    /** returns ID number for reference sequence where alignment's mate was aligned
     */
    int32_t getMateRefID() const override;

    /** returns the number of equal signs in the query sequence - occur after "samtools calmd -e" was run
     */
    uint32_t countEqualSignsInQuerySequence() const override;

    /** returns the sum of the total length of the M and I cigar operations.
     */
    uint32_t sumMandIOperations() const override;

    /** returns number of insertions wrt Query and Reference through the summation of operations D and I in the CIGAR string
     */
    uint32_t sumDandIOperations() const override;

    /** returns tag data
     *
     * @param tag_name
     * @param value return the tags value
     * @return true if tag exists and contains a valid value of values type
     */
    bool getTagData(const std::string &tag_name, int32_t &value) const override;

    /** returns tag data
     *
     * @param tag_name
     * @param value return the tags value
     * @return true if tag exists and contains a valid value of values type
     */
    bool getTagData(const std::string &tag_name, std::string &value) const override;

    /** add a new tag of type "Z"
     */
    void addZTag(const std::string &tag_name, const std::string &value) override;

    /** removes a tag
     */
    void removeTag(const std::string &tag_name) override;

    /** Returns string with name of the reference of an alignment sequence.
     */
    std::string getReferenceName() const override;
};

class BamSeqLibWriter : public BamFileWriter {
private:
    SeqLib::BamWriter writer = SeqLib::BamWriter(SeqLib::BAM);
public:
    /** opens a BAM file for writing and copy headers from the specified reader
     */
    bool openWriter(const std::string &filename, const BamFileReader &reader) override;

    /** saves the alignment to the alignment archive
     */
    bool saveAlignment(const BamAlignmentRecord_ &alignment) override;

    /** closes the current BAM file
     */
    void close() override;
};

class BamSeqLibReader : public BamFileReader {
private:
    SeqLib::BamReader reader;
    SeqLib::BamRecord bamAlignment;
    std::shared_ptr<SeqLib::BamHeader> bamHeader;

    friend bool BamSeqLibWriter::openWriter(const std::string &, const BamFileReader &);

public:
    /** opens a BAM file
     */
    bool openReader(const std::string &filename) override;

    /** retrieves next available alignment
     */
    bool getNextAlignmentRecord(BamAlignmentRecord_ &alignment) override;

    /** closes the current BAM file
     */
    bool close() override;
};

class BamSeqLibUtils : public BamUtils {
private:
#ifdef SEQLIB_1_2
    static ThreadPool_ threadpool;
#endif

    friend bool BamSeqLibWriter::openWriter(const std::string &, const BamFileReader &);
    friend bool BamSeqLibReader::openReader(const std::string &filename);

public:

    BamSeqLibUtils(globalOptions_t &globalOptions);

    /**
     * Sort alignments by QueryName in ascending order.
     *
     * @param alignments
     */
    void sortByQueryNameAscending(std::vector<BamAlignmentRecord_> &alignments) const override;

    /**
     * Sort alignments by Position in ascending order.
     *
     * @param alignments
     */
    void sortByPositionAscending(std::vector<BamAlignmentRecord_> &alignments) const override;

    /**
     * Sort alignments by tag name "sc" in descending order.
     *
     * @param alignments
     */
    void sortByscTagDescending(std::vector<BamAlignmentRecord_> &alignments) const override;
};

#endif
