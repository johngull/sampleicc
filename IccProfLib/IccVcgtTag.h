/* 
 * File:   IccVCGTTag.h
 * Author: Vitaly Bondar
 *
 * Created on 15.11.2012
 */

#ifndef ICCVCGTTag_H
#define	ICCVCGTTag_H

#include "IccTagBasic.h"

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

enum
{
    icSigVCGTType                      = 0x76636774,  /* 'vcgt' */
};

/**
 ***********************************************************************
 * Class: IIccTagFactory
 *
 * Purpose:
 * The VCGT tag type
 ***********************************************************************
 */
class ICCPROFLIB_API IccVCGTTag : public CIccTag
{
public:
    IccVCGTTag();
    IccVCGTTag(const IccVCGTTag& orig);
    IccVCGTTag &operator=(const IccVCGTTag &tag);
    virtual CIccTag *NewCopy() const { return new IccVCGTTag(*this);}
    virtual ~IccVCGTTag();

    virtual icTagTypeSignature GetType() { return icTagTypeSignature(icSigVCGTType); }
    virtual const icChar *GetClassName() { return "IccVCGTTag"; }

    void SetSize(icUInt16Number channels, icUInt16Number entryCount);
    icUInt16Number *operator[](icUInt32Number channel);

    virtual bool Read(icUInt32Number size, CIccIO *pIO);
    virtual bool Write(CIccIO *pIO);
    
    virtual void Describe(std::string &sDescription);
    
    icUInt16Number channels();
    icUInt16Number entryCount();

    virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL) const;
private:

    icUInt16Number channels_;
    icUInt16Number entryCount_;
    icUInt16Number *curves_;
};

#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif

#endif	/* IccVCGTTag_H */

