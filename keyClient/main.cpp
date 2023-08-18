#include<iostream>
#include"clientOP.h"
int m_select()
{
	int nSel = -1;
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n  /*     1.key agree                                            */");
	printf("\n  /*     2.key check                                            */");
	printf("\n  /*     3.key delete                                           */");
	//printf("\n  /*     4.key get                                           */");
	printf("\n  /*     0.quit                                            */");
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n\n  your select:");

	scanf("%d", &nSel);
	while (getchar() != '\n');

	return nSel;
}
int main()
{
	clientOP client("config.json");

	while (1)
	{
		switch (m_select())
		{
		case 1:
			// 秘钥协商
			client.keyAgree();
			break;
		case 2:
			// 秘钥校验
			client.keyCheck();
			break;
		case 3:
			// 秘钥注销
			client.keyDelete();
		default:
			break;

		}
	}

	getchar();
	return 0;
}