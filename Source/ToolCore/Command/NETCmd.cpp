//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"

#include "NETCmd.h"
#include "../AtomicNET/AtomicNETService.h"

namespace ToolCore
{

NETCmd::NETCmd(Context* context) : Command(context)
{

}

NETCmd::~NETCmd()
{

}

bool NETCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    command_ = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1].ToLower() : String::EMPTY;

    if (argument != "net")
    {
        errorMsg = "Unable to parse net command";
        return false;
    }

    if (command_ == "parse")
    {
        assemblyPath_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;

        bool exists = false;

        if (assemblyPath_.Length())
        {
            FileSystem* fs = GetSubsystem<FileSystem>();
            exists = fs->FileExists(assemblyPath_);
        }

        if (!exists)
        {
            errorMsg = ToString("Assembly file not found: %s", assemblyPath_.CString());
            return false;
        }

        return true;
    }
    else
    {
        errorMsg = "Unknown net command";
        return false;
    }


    return true;
}

void NETCmd::Run()
{
    // start the NETService, which means we also need IPC
    IPC* ipc = new IPC(context_);
    context_->RegisterSubsystem(ipc);

    netService_ = new AtomicNETService(context_);
    context_->RegisterSubsystem(netService_);

    if (!netService_->Start())
    {
        Error("Unable to start AtomicNETService");
    }

    Finished();
}

}