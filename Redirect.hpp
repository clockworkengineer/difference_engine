/*
 * File:   Redirect.hpp
 * 
 * Author: Robert Tizzard
 *
 * Created on October 24, 2016, 2:34 PM
 *
 * The MIT License
 *
 * Copyright 2016.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef REDIRECT_HPP
#define REDIRECT_HPP

// STL definitions

#include <iostream>
#include <memory>
#include <fstream>

class Redirect {
    
public:

    Redirect(std::ostream& outStream);
    Redirect(std::ostream& outStream, std::string outfileName, std::ios_base::openmode mode = std::ios_base::out);
    ~Redirect();

    void change(std::string outfileName, std::ios_base::openmode mode = std::ios_base::out);
    void restore(void);

private:

    Redirect() = delete;
    Redirect(const Redirect & orig) = delete;
    Redirect(const Redirect && orig) = delete;

    std::unique_ptr<std::ofstream> fileStream = nullptr;
    std::ostream *outStream;
    std::streambuf *outBuffer;

};
#endif /* REDIRECT_HPP */

