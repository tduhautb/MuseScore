#pragma once

#include "LilyElement.hpp"

namespace Ms
{
class Dynamic;
class LilyDynamic final : public LilyElement
{
  private:
    const Dynamic* _dynamic;

  public:
    LilyDynamic(const Dynamic* dynamic);
    std::ofstream& operator>>(std::ofstream& file) const final;
    std::string name() const
    {
        return "LilyDynamic";
    }
    void log(unsigned int indentation) const final;
};
} // namespace Ms
