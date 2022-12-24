
#ifndef PARSECONFIGFILE_HPP
# define PARSECONFIGFILE_HPP

# include "Parser.hpp"

int		parseConfigFile(int argc, char** argv, std::vector<ServerData>& cData);
void	visitorVisit(const std::vector<ServerData>& cData);

#endif
