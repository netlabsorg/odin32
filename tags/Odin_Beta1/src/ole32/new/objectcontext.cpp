/* $Id: objectcontext.cpp,v 1.1 2000-04-27 22:18:26 davidr Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * COM ObjectContext handling
 * 
 * 22/04/2000
 * 
 * Copyright 2000 David J. Raison
 * 
 */

#include "ole32.h"
#include "oString.h"
#include "oTls.h"
#include "apartment.h"
#include "rpcdce.h"

// ======================================================================
// ObjectContext
// ======================================================================

// ----------------------------------------------------------------------
// ObjectContext::ObjectContext
// ----------------------------------------------------------------------
ObjectContext::ObjectContext()
{
    UuidCreate(&m_Guid);
}

// ----------------------------------------------------------------------
// ObjectContext::IsCompatible
// ----------------------------------------------------------------------
BOOL		ObjectContext::IsCompatible(Attributes * const pTarget) const
{
    // Assume the object context is compatible, until we get into COM+ MTS objects.
    return TRUE;
}

