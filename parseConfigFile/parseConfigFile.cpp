
#include "parseConfigFile.hpp"

int	parseConfigFile(int argc, char** argv, std::vector<ServerData>& cData)
{
	std::string	defPath = "/Users/mac/Desktop/Webserv/parseConfigFile/configFileExamples/simple_config.conf";

	if (argc == 2)
		defPath = argv[1];

	try
	{
		if (argc > 2)
			throw Failure("webserv: [ERROR]: Usage: ./webserv [configuration file]");
		Parser	parser(defPath, cData);
		parser.parserParse();
	}
	catch (Failure& e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	catch (SyntaxError& e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}

	return (0);
}

