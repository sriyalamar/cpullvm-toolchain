//===- DTLTO.h - Distributed ThinLTO functions and classes ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//

<<<<<<< HEAD
#ifndef LLVM_DTLTO_DTLTO_H
#define LLVM_DTLTO_DTLTO_H
=======
#ifndef LLVM_DTLTO_H
#define LLVM_DTLTO_H
>>>>>>> refs/tags/llvmorg-22.1.0-rc1

#include "llvm/LTO/LTO.h"
#include "llvm/Support/MemoryBuffer.h"

namespace llvm {
namespace lto {

<<<<<<< HEAD
// The purpose of this class is to prepare inputs so that distributed ThinLTO
// backend compilations can succeed.
//
// For distributed compilation, each input must exist as an individual bitcode
// file on disk and be loadable via its ModuleID. This requirement is not met
// for archive members, as an archive is a collection of files rather than a
// standalone file. Similarly, for FatLTO objects, the bitcode is stored in a
// section of the containing ELF object file. To address this, the class ensures
// that an individual bitcode file exists for each input (by writing it out if
// necessary) and that the ModuleID is updated to point to it. Module IDs are
// also normalized on Windows to remove short 8.3 form paths that cannot be
// loaded on remote machines.
//
// The class ensures that lto::InputFile objects are preserved until enough of
// the LTO pipeline has executed to determine the required per-module
// information, such as whether a module will participate in ThinLTO.
class DTLTO : public LTO {
  using Base = LTO;

public:
  LLVM_ABI DTLTO(Config Conf, ThinBackend Backend,
                 unsigned ParallelCodeGenParallelismLevel, LTOKind LTOMode,
                 StringRef LinkerOutputFile, bool SaveTemps)
      : Base(std::move(Conf), Backend, ParallelCodeGenParallelismLevel,
             LTOMode),
        LinkerOutputFile(LinkerOutputFile), SaveTemps(SaveTemps) {
    assert(!LinkerOutputFile.empty() && "expected a valid linker output file");
  }

  // Add an input file and prepare it for distribution.
  LLVM_ABI Expected<std::shared_ptr<InputFile>>
  addInput(std::unique_ptr<InputFile> InputPtr) override;

protected:
  // Save the contents of ThinLTO-enabled input files that must be serialized
  // for distribution, such as archive members and FatLTO objects, to individual
  // bitcode files named after the module ID.
  LLVM_ABI llvm::Error serializeInputsForDistribution() override;

  LLVM_ABI void cleanup() override;
=======
class DTLTO : public LTO {
public:
  // Inherit contructors from LTO base class.
  using LTO::LTO;
  ~DTLTO() { removeTempFiles(); }
>>>>>>> refs/tags/llvmorg-22.1.0-rc1

private:
  // Bump allocator for a purpose of saving updated module IDs.
  BumpPtrAllocator PtrAlloc;
  StringSaver Saver{PtrAlloc};

<<<<<<< HEAD
  /// The output file to which this LTO invocation will contribute.
  StringRef LinkerOutputFile;

  /// The normalized output directory, derived from LinkerOutputFile.
  StringRef LinkerOutputDir;

  /// Controls preservation of any created temporary files.
  bool SaveTemps;
=======
  // Removes temporary files.
  LLVM_ABI void removeTempFiles();

  // Determines if a file at the given path is a thin archive file.
  Expected<bool> isThinArchive(const StringRef ArchivePath);

  // Write the archive member content to a file named after the module ID.
  Error saveInputArchiveMember(lto::InputFile *Input);

  // Iterates through all input files and saves their content
  // to files if they are regular archive members.
  Error saveInputArchiveMembers();
>>>>>>> refs/tags/llvmorg-22.1.0-rc1

  // Array of input bitcode files for LTO.
  std::vector<std::shared_ptr<lto::InputFile>> InputFiles;

<<<<<<< HEAD
  // Cache of whether a path refers to a thin archive.
  StringMap<bool> ArchiveIsThinCache;

  // Determines if the file at the given path is a thin archive.
  Expected<bool> isThinArchive(const StringRef ArchivePath);
};

} // namespace lto
} // namespace llvm

#endif // LLVM_DTLTO_DTLTO_H
=======
  // A cache to avoid repeatedly reading the same archive file.
  StringMap<bool> ArchiveFiles;

public:
  // Adds the input file to the LTO object's list of input files.
  // For archive members, generates a new module ID which is a path to a real
  // file on a filesystem.
  LLVM_ABI virtual Expected<std::shared_ptr<lto::InputFile>>
  addInput(std::unique_ptr<lto::InputFile> InputPtr) override;

  // Entry point for DTLTO archives support.
  LLVM_ABI virtual llvm::Error handleArchiveInputs() override;
};
} // namespace lto
} // namespace llvm

#endif // LLVM_DTLTO_H
>>>>>>> refs/tags/llvmorg-22.1.0-rc1
