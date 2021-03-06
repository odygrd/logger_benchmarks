/* This file is part of reckless logging
 * Copyright 2015, 2016 Mattias Flodin <git@codepentry.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "detail/fd_writer.hpp"
#include "posix_error_category.hpp"         // get_posix_error_category

#include <errno.h>      // errno, EINTR
#include <unistd.h>     // write

namespace reckless {
namespace detail {

std::size_t fd_writer::write(void const* pbuffer, std::size_t count, std::error_code& ec) noexcept
{
    char const* p = static_cast<char const*>(pbuffer);
    char const* pend = p + count;
    ec.clear();
    while(p != pend) {
        ssize_t written = ::write(fd_, p, count);
        if(written == -1) {
            if(errno != EINTR) {
                ec.assign(errno, get_posix_error_category());
                break;
            }
        } else {
            p += written;
        }
    }
    return p - static_cast<char const*>(pbuffer);
}

}   // namespace detail
}   // namespace reckless
