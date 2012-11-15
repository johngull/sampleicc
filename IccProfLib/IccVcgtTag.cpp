/* 
 * File:   IccVCGTTag.cpp
 * Author: Vitaly Bondar
 * 
 * Created on 15.11.2012
 */

#include "IccVCGTTag.h"

#include "IccIO.h"

#include <string.h>
#include <stdlib.h>
#include <sstream>

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

const icUInt32Number cmVideoCardGammaTableType = 0;

/**
****************************************************************************
* Name: IccVCGTTag::IccVCGTTag
* 
* Purpose: Constructor
*****************************************************************************
*/
IccVCGTTag::IccVCGTTag() : channels_(0), entryCount_(0), curves_(NULL)
{
}

/**
****************************************************************************
* Name: IccVCGTTag::IccVCGTTag
* 
* Purpose: Copy constructor
* 
*****************************************************************************
*/
IccVCGTTag::IccVCGTTag(const IccVCGTTag& orig)
{
    channels_ = orig.channels_;
    entryCount_ = orig.entryCount_;
    if(channels_ && entryCount_ && orig.curves_)
    {
        curves_ = (icUInt16Number*)calloc(channels_*entryCount_, sizeof(icUInt16Number));
        memcpy(curves_, orig.curves_, channels_*entryCount_*sizeof(icUInt16Number));
    }
    else
        channels_ = entryCount_ = 0;
}

/**
****************************************************************************
* Name: IccVCGTTag::operator=
* 
* Purpose: copy operator
*****************************************************************************
*/
IccVCGTTag &IccVCGTTag::operator=(const IccVCGTTag &tag)
{
    if (&tag == this)
        return *this;

    channels_ = tag.channels_;
    entryCount_ = tag.entryCount_;
    if(channels_ && entryCount_ && tag.curves_)
    {
        curves_ = (icUInt16Number*)calloc(channels_*entryCount_, sizeof(icUInt16Number));
        memcpy(curves_, tag.curves_, channels_*entryCount_*sizeof(icUInt16Number));
    }
    else
        channels_ = entryCount_ = 0;

    return *this;
}

/**
****************************************************************************
* Name: IccVCGTTag::~IccVCGTTag
* 
* Purpose: Destructor
* 
*****************************************************************************
*/
IccVCGTTag::~IccVCGTTag()
{
    if(curves_)
        free(curves_);
}

/**
****************************************************************************
* Name: IccVCGTTag::SetSize
* 
* Purpose: Sets the size of the correction lut.
* 
* Args: 
*  channels - number of channels,
*  entryCount - number of elements per channel
*****************************************************************************
*/
void IccVCGTTag::SetSize(icUInt16Number channels, icUInt16Number entryCount)
{
  if (channels_==channels && entryCount_==entryCount)
    return;

  if (channels==0 || entryCount==0)
    return;

  channels_ = channels;
  entryCount_ = entryCount;
  if (curves_)
      free(curves_);
  curves_ = (icUInt16Number*)malloc(channels_*entryCount_*sizeof(icUInt16Number));
}

/**
****************************************************************************
* Name: IccVCGTTag::operator[]
* 
* Purpose: Array-like access to channels
* 
* Return: 
*  pointer to the array of values which represent channel
*****************************************************************************
*/
icUInt16Number *IccVCGTTag::operator[](icUInt32Number channel)
{
    return curves_ + channel*entryCount_;
}

/**
****************************************************************************
* Name: IccVCGTTag::Read
* 
* Purpose: Read in the tag contents into a data block
* 
* Args:
*  size - # of bytes in tag,
*  pIO - IO object to read tag from
* 
* Return: 
*  true = successful, false = failure
*****************************************************************************
*/
bool IccVCGTTag::Read(icUInt32Number size, CIccIO *pIO)
{
    icTagTypeSignature sig;

    if (sizeof(icTagTypeSignature) +
        2*sizeof(icUInt32Number) +
        3*sizeof(icUInt16Number) > size)
            return false;

    if (!pIO) 
        return false;

    if (!pIO->Read32(&sig))
        return false;

    if (!pIO->Read32(&m_nReserved))
        return false;

    icUInt32Number type;

    if (!pIO->Read32(&type))
        return false;

    if(type!=cmVideoCardGammaTableType)
        return false;

    //sizes
    icUInt16Number ch, ec, esz;
    if (!pIO->Read16(&ch))
        return false;

    if (!pIO->Read16(&ec))
        return false;

    if (!pIO->Read16(&esz))
        return false;

    if(esz != sizeof(icUInt16Number))
        return false;

    SetSize(ch, ec);

    if (curves_)
        free(curves_);
    curves_ = (icUInt16Number*)malloc(channels_*entryCount_*sizeof(icUInt16Number));
    if (pIO->Read16(curves_, channels_*entryCount_)!=(icInt32Number)(channels_*entryCount_))
        return false;

    return true;
}

/**
****************************************************************************
* Name: IccVCGTTag::Write
* 
* Purpose: Write the tag to a file
* 
* Args: 
*  pIO - The IO object to write tag to.
* 
* Return: 
*  true = succesful, false = failure
*****************************************************************************
*/
bool IccVCGTTag::Write(CIccIO *pIO)
{
    icTagTypeSignature sig = GetType();

    if (!pIO)
        return false;

    if (!pIO->Write32(&sig))
        return false;

    if (!pIO->Write32(&m_nReserved))
        return false;

    // VideoCardGamma
    icUInt32Number tagtype = cmVideoCardGammaTableType;
    if (!pIO->Write32(&tagtype))
        return false;

    // VideoCardGammaTable
    if (!pIO->Write16(&channels_))
        return false;

    if (!pIO->Write16(&entryCount_))
        return false;

    icUInt16Number entrysz = sizeof(icUInt16Number);
    if (!pIO->Write16(&entrysz))
        return false;

    if(curves_)
        if (pIO->Write16(curves_, channels_*entryCount_)!=(icInt32Number)(channels_*entryCount_))
            return false;
    
    return true;
}

/**
****************************************************************************
* Name: IccVCGTTag::Describe
* 
* Purpose: Dump data associated with the tag to a string
* 
* Args: 
*  sDescription - string to concatenate tag dump to
*****************************************************************************
*/
void IccVCGTTag::Describe(std::string &sDescription)
{
    std::stringstream ss;
    ss << "VCGT LUT" << std::endl;
    for(int i=0;i<channels_;++i)
    {
        ss << "Channel#" << i << std::endl;
        for(int j=0;j<entryCount_;++j)
            ss << curves_[j+i*entryCount_] << std::endl;
        ss << std::endl;
    }
    ss << std::endl;
    sDescription += ss.str();
}

/**
******************************************************************************
* Name: IccVCGTTag::Validate
* 
* Purpose: Check tag data validity. (not implemented yet)
* 
* Args: 
*  sig = signature of tag being validated,
*  sReport = String to add report information to
* 
* Return: 
*  icValidateStatusOK if valid, or other error status.
******************************************************************************
*/
icValidateStatus IccVCGTTag::Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile) const
{
    return icValidateOK;
}

/**
****************************************************************************
* Name: IccVCGTTag::channels
* 
* Purpose: get channels number
* 
* Return: 
*  channels count
*****************************************************************************
*/
icUInt16Number IccVCGTTag::channels()
{
    return channels_;
}

/**
****************************************************************************
* Name: IccVCGTTag::entryCount
* 
* Purpose: get elements count per channel
* 
* Return: 
*  elements count
*****************************************************************************
*/
icUInt16Number IccVCGTTag::entryCount()
{
    return entryCount_;
}

#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif