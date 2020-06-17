//
// Created by ArcticMarmoset on 2019-11-30.
//

#include "custom/String.h"

Custom::String::String()
{
    // String should always have at least the null char
    curLen_ = 1;
    // Initial max length of 19 with null char
    maxLen_ = 19;
    // Allocate char array of size 19
    string_ = new char[maxLen_];
    // Terminate string_ with the null char
    string_[0] = '\0';
    // Initialise refCount_
    refCount_ = new int(0);
}

Custom::String::~String()
{
    if (*refCount_ == 0)
    {
        delete [] string_;
        delete refCount_;
        return;
    }

    // Otherwise, decrement refCount
    (*refCount_)--;
}

Custom::String::String(const char *str) : String()
{
    // arrayLength is the number of chars plus the null char
    int arrayLength = String::LengthOf(str) + 1;

    // If arrayLength is 1, then str is empty
    if (arrayLength == 1)
    {
        return;
    }

    // If arrayLength exceeds 19, we need a new array
    if (arrayLength > maxLen_)
    {
        // Update maxLen_ with arbitrary buffer
        maxLen_ += arrayLength;
        // Free the old one
        delete [] string_;
        // Allocate the new one
        string_ = new char[maxLen_];
    }

    // Update curLen_
    curLen_ = arrayLength;

    // Copy over chars from str
    for (int i = 0; i < curLen_; i++)
    {
        string_[i] = str[i];
    }
}

Custom::String::String(const Custom::String &str)
{
    // Copy constructor so point string_ to same memory
    string_ = str.string_;
    // Point refCount_ to same memory
    refCount_ = str.refCount_;
    // Update lengths
    curLen_ = str.curLen_;
    maxLen_ = str.maxLen_;
    // And increment refCount
    (*refCount_)++;
}

int Custom::String::LengthOf(const char *str)
{
    if (!str)
    {
        return 0;
    }

    char c = str[0];

    if (!c)
    {
        return 0;
    }

    int i = 1;
    while ((c = str[i]))
    {
        i++;
    }

    return i;
}

int Custom::String::Length() const
{
    return curLen_ - 1;
}

char *Custom::String::ToCharArray() const
{
    char *string = new char[curLen_];
    for (int i = 0; i < curLen_; i++)
    {
        string[i] = string_[i];
    }
    return string;
}

bool Custom::String::IsEmpty() const
{
    return curLen_ == 1;
}

int Custom::String::IndexOf(const char c, int fromIndex) const
{
    if (curLen_ == 1)
    {
        return -1;
    }

    // Loop from fromIndex to curLen_
    for (int i = fromIndex; i < curLen_; i++)
    {
        if (string_[i] == c)
        {
            return i;
        }
    }
    return -1;
}

int Custom::String::IndexOf(const char *str) const
{
    int strLength = String::LengthOf(str);

    if (curLen_ == 1 || strLength == 0)
    {
        return -1;
    }

    for (int i = 0; i < curLen_; i++)
    {
        // Look for a first-char match
        if (string_[i] != str[0])
        {
            continue;
        }

        // There was a match so compare the rest
        for (int j = 1; j <= strLength; j++)
        {
            // If we are already at strLength, all chars have matched
            if (j == strLength)
            {
                // Increment i to match j
                i++;
                // Return the index of the first char
                return i - j;
            }

            // Otherwise, get the next char
            i++;

            // If this pair does not match, look for the next first-char match
            if (string_[i] != str[j])
            {
                break;
            }
        }
    }

    // A match was not found
    return -1;
}

int Custom::String::IndexOf(const Custom::String &str) const
{
    return IndexOf(str.string_);
}

bool Custom::String::Contains(const char c) const
{
    return IndexOf(c) != -1;
}

bool Custom::String::Contains(const char *str) const
{
    return IndexOf(str) != -1;
}

bool Custom::String::Contains(const Custom::String &str) const
{
    return IndexOf(str) != -1;
}

Custom::String &Custom::String::PushBack(char c)
{
    bool canFit = curLen_ + 1 <= maxLen_;
    bool isShared = *refCount_ > 0;

    // If we can fit 1 more char, and the memory at string_ is not shared,
    // then just append the char and re-add the null char
    if (canFit && !isShared)
    {
        string_[curLen_ - 1] = c;
        string_[curLen_] = '\0';
        curLen_++;
        return *this;
    }

    // In all other cases, we need to dynamically allocate a new array

    // But the size of it must first be determined
    if (!canFit)
    {
        maxLen_ = maxLen_ * 2;
    }

    // Dynamically allocate the new array
    char *newString = new char[maxLen_];

    // Copy current string into newString
    for (int i = 0; i < curLen_; i++)
    {
        newString[i] = string_[i];
    }

    // Add the char
    newString[curLen_ - 1] = c;
    newString[curLen_] = '\0';

    // Increment curLen_
    curLen_++;

    // If memory at string_ was shared, decrement refCount_ and start anew
    if (isShared)
    {
        (*refCount_)--;
        refCount_ = new int(0);
    }
    // Otherwise, free memory at string_
    else
    {
        delete [] string_;
    }

    // Update string_
    string_ = newString;
    return *this;
}

char Custom::String::PopBack()
{
    // Cannot pop if string is empty
    if (curLen_ == 1)
    {
        return '\0';
    }

    // If string_ is shared, create a new string, decrement refCount_
    if (*refCount_ > 0)
    {
        char *newString = new char[maxLen_];
        for (int i = 0; i < curLen_; i++)
        {
            newString[i] = string_[i];
        }
        (*refCount_)--;
        refCount_ = new int(0);
        string_ = newString;
    }

    // Set the char at curLen_ - 1 to null
    char c = string_[curLen_ - 1];
    string_[curLen_ - 1] = '\0';
    curLen_--;
    return c;
}

Custom::String &Custom::String::Append(char c)
{
    return PushBack(c);
}

char **Custom::String::Split(char delimiter) const
{
    // Cannot split if string is empty
    if (curLen_ == 1)
    {
        return nullptr;
    }

    // Calculate number of chunks
    int chunkCount = 1;
    for (int i = 0; i < curLen_; i++)
    {
        if (string_[i] == delimiter)
        {
            chunkCount++;
        }
    }

    // Cannot split with no chunks
    // ? Maybe return array of 1 string
    if (chunkCount == 1)
    {
        return nullptr;
    }

    char **chunks = new char *[chunkCount + 1];

    int prevIndex = 0;
    int delIndex;
    for (int i = 0; i < chunkCount; i++)
    {
        // Get index of delimiter, starting from the prevIndex
        delIndex = IndexOf(delimiter, prevIndex);

        int chunkSize;
        if (delIndex > prevIndex)
        {
            chunkSize = delIndex - prevIndex;
        }
        else
        {
            chunkSize = curLen_ - prevIndex - 1;
        }

        char *chunk = new char[chunkSize + 1];

        // Copy string starting from prevIndex and ending at next delimiter
        for (int j = 0; j < chunkSize; j++)
        {
            chunk[j] = string_[prevIndex + j];
        }

        // Terminate the string
        chunk[chunkSize] = '\0';

        // Update prevIndex to next delimiter index + 1
        prevIndex = delIndex + 1;

        // Assign this chunk to the array
        chunks[i] = chunk;
    }

    chunks[chunkCount] = nullptr;
    return chunks;
}

Custom::String &Custom::String::operator=(const char *str)
{
    // TODO: Finish implementing
    return *this;
}

Custom::String &Custom::String::operator=(const Custom::String &str)
{
    if (this == &str)
    {
        return *this;
    }

    if (*refCount_ == 0)
    {
        delete [] string_;
    }

    refCount_ = str.refCount_;
    string_ = str.string_;
    curLen_ = str.curLen_;
    maxLen_ = str.maxLen_;
    (*refCount_)++;
    return *this;
}

bool Custom::String::operator==(const Custom::String &str) const
{
    if (string_ == str.string_)
    {
        return true;
    }

    if (curLen_ != str.curLen_)
    {
        return false;
    }

    for (int i = 0; i < curLen_; i++)
    {
        if (string_[i] != str.string_[i])
        {
            return false;
        }
    }

    return true;
}
