
#include <iostream>
#include <string>

namespace PicturePerfectOpenCVOperators
{

class Alias : public Operator
{
	private :

    public :
		Alias() {};

		virtual ResultType  process(const strings& tokens, PacketStore& packets)
		{
			assertNumberOfArgs(tokens, 3);

			packets.createAlias(tokens[2], tokens[1]);

			return RES_NEXT;
		}
};

}