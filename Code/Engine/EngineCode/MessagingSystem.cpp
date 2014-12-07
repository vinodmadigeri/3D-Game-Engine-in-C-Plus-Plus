
#include "Precompiled.h"

#include "MessagingSystem.h"


namespace Engine
{
	namespace MessagingSystem
	{
		std::vector<Message > mMessageList;
		/******************************************************************************
		 Function     : CreateMessage
		 Description  : Function to create message and added it to message list
		 Input        : const HashedString &i_Message
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void CreateMessage(const HashedString &i_Message)
		{
			for (unsigned int i = 0; i < mMessageList.size(); i++)
			{
				if (mMessageList.at(i).mMessage == i_Message)
				{
					return;
				}
			}

			Message NewMessage;

			NewMessage.mMessage = i_Message;

			mMessageList.push_back(NewMessage);
		}

		/******************************************************************************
		 Function     : RegisterMessageHandler
		 Description  : Function to register message handler
		 Input        : const HashedString &i_Message, const MessageHandler i_Handler
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void RegisterMessageHandler(const HashedString &i_Message, const MessageHandler i_Handler)
		{
			for (unsigned int i = 0; i < mMessageList.size(); i++)
			{
				if (mMessageList.at(i).mMessage == i_Message)
				{
					mMessageList.at(i).mHandlers.push_back(i_Handler);

					return;
				}
			}

			Message NewMessage;
			NewMessage.mMessage = i_Message;
			NewMessage.mHandlers.push_back(i_Handler);

			mMessageList.push_back(NewMessage);
		}

		/******************************************************************************
		 Function     : UnRegisterMessageHandler
		 Description  : Function to unregister message handler
		 Input        : const HashedString &i_Message, const MessageHandler i_Handler
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void UnRegisterMessageHandler(const HashedString &i_Message, const MessageHandler i_Handler)
		{
			for (unsigned int i = 0; i < mMessageList.size(); i++)
			{
				if (mMessageList.at(i).mMessage == i_Message)
				{
					for (unsigned int j = 0; j < mMessageList.at(i).mHandlers.size(); j++)
					{
						if (mMessageList.at(i).mHandlers.at(j) == i_Handler)
						{
							mMessageList.at(i).mHandlers.erase(mMessageList.at(i).mHandlers.begin() + j);
							return;
						}
					}
				}
			}

			Message NewMessage;
			NewMessage.mMessage = i_Message;
			NewMessage.mHandlers.push_back(i_Handler);

			mMessageList.push_back(NewMessage);
		}

		/******************************************************************************
		 Function     : BroadcastMessage
		 Description  : Broadcast message function that calls the handler on receiving 
						appropriate message.
		 Input        : const HashedString &i_Message, void *i_Data
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void BroadcastMessage(const HashedString &i_Message, void *i_Data)
		{
			for (unsigned int i = 0; i < mMessageList.size(); i++)
			{
				if (mMessageList.at(i).mMessage == i_Message)
				{
					for (unsigned int j = 0; j < mMessageList.at(i).mHandlers.size(); j++)
					{
						(mMessageList.at(i).mHandlers.at(j))(i_Data);
					}
					
					return;
				}
			}

			assert(false);
		}
	}
}