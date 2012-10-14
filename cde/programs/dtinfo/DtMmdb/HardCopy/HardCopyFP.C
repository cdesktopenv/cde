/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $TOG: HardCopyFP.C /main/4 1998/04/17 11:46:58 mgreess $


#include "HardCopyFP.h"

char* loutFeatureProcessor::empty_string()
{
   char* x = new char[1]; x[0] = 0;
   return x;
}

char* 
loutFeatureProcessor::prepend(const char* header, const char* body)
{
   char* x = new char[strlen(header)+strlen(body)+1];
   strcpy(x, header);
   strcat(x, body);
   return x;
}

void 
loutFeatureProcessor::handleData(const char *data, unsigned int size, ostream& out)
{
   for (int i=0; i<size; i++ )
     out << data[i];
}

char* literalBuffer = new char[1024];
int literalBufferSize = 1024;

const char* loutFeatureProcessor::convertToLiteral(const char* str)
{
   int size = strlen(str);
   for (int i=0; i<size; i++ )
     if ( str[i] == '\\' ||  str[i] == '"' ) {

        if ( literalBufferSize < 2*size + 3 ) {
           literalBufferSize = 2*size + 3;
           literalBufferSize *= 2;
           delete literalBuffer;
           literalBuffer = new  char[literalBufferSize];
        }    

        int j = 0;
        literalBuffer[j++] = '"';
        for (int i=0; i<size; i++ ) {
           if ( str[i] == '\\' ||  str[i] == '"' ) {
               literalBuffer[j++] = '\\';
           }
           literalBuffer[j++] = str[i];
        }

        literalBuffer[j++] = '"';
        literalBuffer[j] = 0;
       
        return literalBuffer;
     }
   return str;
}


const char* loutFeatureProcessor::stringToCharPtr(const FeatureValue* f)
{
   if ( f ) {
      if ( f -> type() == FeatureValue::string ) {
         const char* x = *f;
         return x;
      } else 
         throw(CASTHCREXCEPT hardCopyRendererException());
   }
   return 0;
}

unsigned int 
loutFeatureProcessor::dimensionToFloat(
		float& y, 
		FeatureValueDimension::Unit& unitOfY,
		const FeatureValue* f,
		FeatureValueDimension::Unit u
				      )
{
   if ( f ) {
      if ( f -> type() == FeatureValue::dimension ) {
        FeatureValueDimension* x = (FeatureValueDimension*)f;

        unitOfY = x -> unit();

        if ( u == FeatureValueDimension::NONE ) {
           if ( x -> unit() == FeatureValueDimension::PICA ) {
	      y = x -> getValue(FeatureValueDimension::POINT);
              unitOfY = FeatureValueDimension::POINT;
           } else
              y = *x;
        } else {
           y = x -> getValue(u);
	}
        return 1;
      } else
         throw(CASTHCREXCEPT hardCopyRendererException());
   } else {
      y = 0;
      return 0;
   }
}

const char* 
loutFeatureProcessor::dimensionToCharPtr(const FeatureValue* f,
			FeatureValueDimension::Unit u)
{
   static char dBuf[50];
   float y;
   FeatureValueDimension::Unit unitOfY;

   if ( dimensionToFloat(y, unitOfY, f, u) ) {

        FeatureValueDimension::Unit unit;

        if ( u != FeatureValueDimension::NONE ) {
	   unitOfY = u;
	}

        switch ( unitOfY ) {
          case FeatureValueDimension::INCH:
            sprintf(dBuf, "%.2fi", y);
            break;

          case FeatureValueDimension::PICA:
            throw(CASTHCREXCEPT hardCopyRendererException());
            break;

	  case FeatureValueDimension::POINT:
            sprintf(dBuf, "%.2fp", y);
            break;

	  case FeatureValueDimension::CM:
            sprintf(dBuf, "%.2fc", y);
            break;

	  case FeatureValueDimension::PIXEL:
            throw(CASTHCREXCEPT hardCopyRendererException());
            break;

          default:
            throw(CASTHCREXCEPT hardCopyRendererException());
        }
        return (const char*)dBuf;
   } else 
      return 0;
}

const char*
featureProcessor::getFeatureValue(const Feature* f)
{
   if ( f )
      return stringToCharPtr(f -> value());
   else
      return 0;
}

FeatureValueArray* 
featureProcessor::getFeatureValueArray(const Feature* f)
{
  if ( f ) {
     if ( f -> value() -> type() == FeatureValue::array ) 
        return (FeatureValueArray*)(f -> value());
     else {
        MESSAGE(cerr, form("%s is not of an array type.", f -> name().name()));
        throw(CASTHCREXCEPT hardCopyRendererException());
     }
  } else
     return 0;
}

unsigned int
featureProcessor::getDimensionValue(float& x, const Feature* f, 
			FeatureValueDimension::Unit u)
{
   FeatureValueDimension::Unit unit;

   if ( f )
      return dimensionToFloat(x, unit, f -> value(), u);
   else
      return 0;
}

const char*
featureProcessor::getDimensionValue(const Feature* f, 
			FeatureValueDimension::Unit u)
{
   if ( f )
      return dimensionToCharPtr(f -> value(), u);
   else
      return 0;
}

unsigned int
featureProcessor::getFeatureValue(int& x, const Feature* f)
{
   if ( f ) {
      x = *(f -> value());
      return true;
   } else
      return false;
}

unsigned int
featureProcessor::getFeatureValue(float& x, const Feature* f)
{
   if ( f ) {
      x = *(f -> value());
      return true;
   } else
      return false;
}

const FeatureSet *
featureProcessor::getFeatureSetValue(const Feature* f)
{
  if ( f && f -> value() -> type() == FeatureValue::featureset ) 
    return ((const FeatureValueFeatureSet *)f->value())->value();
  else
    return 0;
}

FeatureValue* 
loutFeatureProcessor::evaluate(const char*) 
{
   MESSAGE(cerr, "featureProcessor::evaluate(): FP specific function should be called.");
   throw(CASTHCREXCEPT hardCopyRendererException());
   return 0;
}

unsigned int
loutFeatureProcessor::accept(const char*, const Expression*)
{
   MESSAGE(cerr, "featureProcessor::accept(): FP specific function should be called");
   throw(CASTHCREXCEPT hardCopyRendererException());
   return false;
}

void loutFeatureProcessor::clear()
{
   MESSAGE(cerr, "featureProcessor::clear(): FP specific function should be called");
   throw(CASTHCREXCEPT hardCopyRendererException());
}

void 
loutFeatureProcessor::preEvaluate(const Element& e)
{
   MESSAGE(cerr, "featureProcessor::preEvaluate(): FP specific function should be called");
   throw(CASTHCREXCEPT hardCopyRendererException());
}

void 
loutFeatureProcessor::postEvaluate(const Element& e)
{
   MESSAGE(cerr, "featureProcessor::postEvaluate(): FP specific function should be called");
   throw(CASTHCREXCEPT hardCopyRendererException());
}
