/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
// .NAME vtkDataObject - general representation of visualization data
// .SECTION Description
// vtkDataObject is an general representation of visualization data. It serves
// to encapsulate instance variables and methods for visualization network 
// execution, as well as representing data consisting of a field (i.e., just
// an unstructured pile of data). This is to be compared with a vtkDataSet,
// which is data with geometric and/or topological structure.
//
// vtkDataObjects are used to represent arbitrary repositories of data via the
// vtkFieldData instance variable. These data must be eventually mapped into a
// concrete subclass of vtkDataSet before they can actually be displayed.
//
// .SECTION See Also
// vtkDataSet vtkFieldData vtkDataObjectSource vtkDataObjectFilter
// vtkDataObjectMapper vtkDataObjectToDataSet 
// vtkFieldDataToAttributeDataFilter

#ifndef __vtkDataObject_h
#define __vtkDataObject_h

#include "vtkObject.h"
#include "vtkFieldData.h"

class vtkSource;

#define VTK_PIECES_EXTENT   0
#define VTK_3D_EXTENT       1

class VTK_EXPORT vtkDataObject : public vtkObject
{
public:
  static vtkDataObject *New();

  const char *GetClassName() {return "vtkDataObject";};
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create concrete instance of this data object.
  virtual vtkDataObject *MakeObject() {return new vtkDataObject;}
  

  // Description:
  // Set/Get the source object creating this data object.
  vtkGetObjectMacro(Source,vtkSource);
  void SetSource(vtkSource *s);
  
  // Description:
  // Data objects are composite objects and need to check each part for MTime.
  // The information object also needs to be considered.
  unsigned long int GetMTime();

  // Rescription:
  // Restore data object to initial state,
  virtual void Initialize();

  // Description:
  // Release data back to system to conserve memory resource. Used during
  // visualization network execution.  Releasing this data does not make 
  // down-stream data invalid, so it does not modify the MTime of this 
  // data object.
  void ReleaseData();

  // Description:
  // Return flag indicating whether data should be released after use  
  // by a filter.
  int ShouldIReleaseData();

  // Description:
  // Get the flag indicating the data has been released.
  vtkGetMacro(DataReleased,int);
  
  // Description:
  // Turn on/off flag to control whether this object's data is released
  // after being used by a filter.
  vtkSetMacro(ReleaseDataFlag,int);
  vtkGetMacro(ReleaseDataFlag,int);
  vtkBooleanMacro(ReleaseDataFlag,int);

  // Description:
  // Turn on/off flag to control whether every object releases its data
  // after being used by a filter.
  static void SetGlobalReleaseDataFlag(int val);
  void GlobalReleaseDataFlagOn() {this->SetGlobalReleaseDataFlag(1);};
  void GlobalReleaseDataFlagOff() {this->SetGlobalReleaseDataFlag(0);};
  static int GetGlobalReleaseDataFlag();

  // Description:
  // Assign or retrieve field data to this data object.
  vtkSetObjectMacro(FieldData,vtkFieldData);
  vtkGetObjectMacro(FieldData,vtkFieldData);
  
  // Handle the source/data loop.
  void UnRegister(vtkObject *o);

  // Description:
  // Get the net reference count. That is the count minus
  // any self created loops. This is used in the Source/Data
  // registration to properly free the objects.
  virtual int GetNetReferenceCount() {return this->ReferenceCount;};

  // Description:
  // Provides opportunity for the data object to insure internal 
  // consistency before access. Also causes owning source/filter 
  // (if any) to update itself. The Update() method is composed of 
  // UpdateInformation(), PropagateUpdateExtent(), 
  // TriggerAsynchronousUpdate(), and UpdateData().
  virtual void Update();

  // Description:
  // WARNING: INTERNAL METHOD - NOT FOR GENERAL USE. 
  // THIS METHOD IS PART OF THE PIPELINE UPDATE FUNCTIONALITY.
  // Update all the "easy to update" information about the object such
  // as the extent which will be used to control the update.
  // This propagates all the way up then back down the pipeline.
  // As a by-product the PipelineMTime is updated.
  virtual void UpdateInformation();

  // Description:
  // WARNING: INTERNAL METHOD - NOT FOR GENERAL USE. 
  // THIS METHOD IS PART OF THE PIPELINE UPDATE FUNCTIONALITY.
  // The update extent for this object is propagated up the pipeline.
  // This propagation may early terminate based on the PipelineMTime.
  virtual void PropagateUpdateExtent();

  // Description:
  // WARNING: INTERNAL METHOD - NOT FOR GENERAL USE. 
  // THIS METHOD IS PART OF THE PIPELINE UPDATE FUNCTIONALITY.
  // Propagate back up the pipeline for ports and trigger the update on the
  // other side of the port to allow for asynchronous parallel processing in
  // the pipeline.
  // This propagation may early terminate based on the PipelineMTime.
  virtual void TriggerAsynchronousUpdate();

  // Description:
  // WARNING: INTERNAL METHOD - NOT FOR GENERAL USE. 
  // THIS METHOD IS PART OF THE PIPELINE UPDATE FUNCTIONALITY.
  // Propagate the update back up the pipeline, and perform the actual 
  // work of updating on the way down. When the propagate arrives at a
  // port, block and wait for the asynchronous update to finish on the
  // other side.
  // This propagation may early terminate based on the PipelineMTime.
  virtual void UpdateData();

  // Description:
  // Get the maximum size of the pipeline. Should only be called after
  // UpdateInformation() and PropagateUpdateExtent() have both been called.
  // The size is returned in kilobytes.
  unsigned long GetEstimatedPipelineMemorySize();

  // Description:
  // Get the estimated size of this data object itself. Should be called
  // after UpdateInformation() and PropagateUpdateExtent() have both been 
  // called. Should be overridden in a subclass - otherwise the default
  // is to assume that this data object requires no memory.
  // The size is returned in kilobytes.
  virtual unsigned long GetEstimatedMemorySize();

  // Description:
  // Propogate the computation of the maximum size of the pipeline.
  // The first size returned is the size of the pipeline after the source
  // has executed (and has therefore had a chance to release any of its
  // input data). The second size returned is the estimated size of this
  // data object according to the source. If this is structured data, then
  // the source likely asked this object for its estimated size. If it
  // is unstructured data, then the source should have made its own
  // prediction as to its output size. The third size returned is the
  // maximum pipeline size encounted upstream during this propagation.
  // All sizes are in kilobytes.
  void ComputeEstimatedPipelineMemorySize( unsigned long sizes[3] );

  // Description:
  // A generic way of specifying an update extent.  Subclasses
  // must decide what a piece is.
  virtual void SetUpdateExtent(int vtkNotUsed(piece),int vtkNotUsed(numPieces))
    { vtkErrorMacro("Subclass did not implement 'SetUpdateExtent'");}
  
  // Description:
  // Set the update extent for data objects that use 3D extents. Using this
  // method on data objects that set extents as pieces (such as vtkPolyData or
  // vtkUnstructuredGrid) has no real effect.
  // Don't use the set macro to set the update extent
  // since we don't want this object to be modified just due to
  // a change in update extent.
  virtual void SetUpdateExtent(int x1, int x2, int y1, int y2, int z1, int z2);
  virtual void SetUpdateExtent( int ext[6] );
  vtkGetVector6Macro( UpdateExtent, int );

  // Description:
  // Return class name of data type. This is one of VTK_STRUCTURED_GRID, 
  // VTK_STRUCTURED_POINTS, VTK_UNSTRUCTURED_GRID, VTK_POLY_DATA, or
  // VTK_RECTILINEAR_GRID (see vtkSetGet.h for definitions).
  // THIS METHOD IS THREAD SAFE
  virtual int GetDataObjectType() {return VTK_DATA_OBJECT;}
  
  // Description:
  // Used by Threaded ports to determine if they should initiate an
  // asynchronous update (still in development).
  unsigned long GetUpdateTime();

  // Description:
  // If the whole input extent is required to generate the requested output
  // extent, this method can be called to set the input update extent to the
  // whole input extent. This method assumes that the whole extent is known
  // (that UpdateInformation has been called)
  void SetUpdateExtentToWholeExtent();

  void SetPipelineMTime(unsigned long time) {this->PipelineMTime = time; }
  vtkGetMacro(PipelineMTime, unsigned long);

  // Description:
  // Return the actual size of the data in kilobytes. This number
  // is valid only after the pipeline has updated. The memory size
  // returned is guaranteed to be greater than or equal to the
  // memory required to represent the data (e.g., extra space in
  // arrays, etc. are not included in the return value).
  virtual unsigned long GetActualMemorySize();

  // Description:
  // Copy the generic information (WholeExtent or MaximumNumberOfPieces)
  void CopyInformation( vtkDataObject *data );

  // Description:
  // By default, there is no type specific information
  virtual void CopyTypeSpecificInformation( vtkDataObject *data ) 
    {this->CopyInformation( data );};
  
  // Description:
  // Set / Get the update piece and the update number of pieces. Similar
  // to update extent in 3D.
  vtkSetMacro( UpdatePiece, int );
  vtkGetMacro( UpdatePiece, int );
  vtkSetMacro( UpdateNumberOfPieces, int );
  vtkGetMacro( UpdateNumberOfPieces, int );

  // Description:
  // Get the maximum number of pieces this data can be broken into
  vtkGetMacro( MaximumNumberOfPieces, int );

  // Description:
  // Get the whole extent of this data object
  vtkGetVector6Macro( WholeExtent, int );

  // Description:
  // This method is called by the source when it executes to generate data.
  // It is sort of the opposite of ReleaseData.
  // It sets the DataReleased flag to 0, and sets a new UpdateTime.
  void DataHasBeenGenerated();


protected:

  vtkDataObject();
  ~vtkDataObject();
  vtkDataObject(const vtkDataObject&) {};
  void operator=(const vtkDataObject&) {};

  // General field data associated with data object      
  vtkFieldData  *FieldData;  

  // Who generated this data as output?
  vtkSource     *Source;     

  // Keep track of data release during network execution
  int DataReleased; 

  // Description:
  // Default behavior is to make sure that the update extent lies within
  // the whole extent. If it does not, an error condition occurs and this
  // method returns 0. If it is ok, then 1 is returned. Since uninitialized
  // extents are initialized to the whole extent during UpdateInformation()
  // there should not be errors. If a data object subclass wants to try to 
  // take care of errors silently, then this method should be overridden.
  virtual int VerifyUpdateExtent();

  // Description:
  // If the update extent does not lie within the extent, this method will
  // release the data and set the extent to be the update extent.
  // Otherwise, nothing changes. If a subclass wants another behavior, 
  // then this method should be overridden (as it is in vtkStructuredPoints)
  virtual void ModifyExtentForUpdateExtent();

  // The ExtentType will be left as VTK_PIECES_EXTENT for data objects 
  // such as vtkPolyData and vtkUnstructuredGrid. The ExtentType will be 
  // changed to VTK_3D_EXTENT for data objects with 3D structure such as 
  // vtkImageData (and its subclass vtkStructuredPoints), vtkRectilinearGrid,
  // and vtkStructuredGrid. The default is the have an extent in pieces,
  // with only one piece (no streaming possible).
  virtual int GetExtentType() { return VTK_PIECES_EXTENT; };

  // If the ExtentType is VTK_3D_EXTENT, then these three extent variables
  // represent the whole extent, the extent currently in memory, and the
  // requested update extent. The extent is given as 3 min/max pairs.
  int WholeExtent[6];
  int Extent[6];
  int UpdateExtent[6];

  // If the ExtentType is VTK_PIECES_EXTENT, then these three variables 
  // represent the maximum number of pieces that the data object can be
  // broken into, which piece out of how many is currently in the extent,
  // and the number of pieces and the specific piece requested for the 
  // update. Data objects that do not support any division
  // of the data (such as a vtkPiecewiseFunction) can simply leave the 
  // MaximumNumberOfPieces as 1. The NumberOfPieces and Piece are similar
  // to the Extent. The UpdateNumberOfPieces and UpdatePiece are similar to
  // the UpdateExtent. The WholeExtent is always piece = 0 and number of 
  // pieces = 1;
  int MaximumNumberOfPieces;
  int NumberOfPieces;
  int Piece;
  int UpdateNumberOfPieces;
  int UpdatePiece;

  // Data will release after use by a filter if this flag is set
  int ReleaseDataFlag; 

  // When was this data last generated?
  vtkTimeStamp UpdateTime;  

  // A guess at how much memory would be consumed by the data object
  // if the WholeExtent were updated.
  unsigned long EstimatedWholeMemorySize;

  // The Maximum MTime of all upstream filters and data objects.
  // This does not include the MTime of this data object.
  unsigned long PipelineMTime;

  // How many upstream filters are local to the process.
  // This will have to change to a float for Kens definition of locality.
  float Locality;  

  // Support for processing series of data sets.
  int SeriesLength;
};

#endif

