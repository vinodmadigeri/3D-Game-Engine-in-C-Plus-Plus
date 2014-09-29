namespace Engine
{
	inline float GenerateRandomNumber(const int iMinRange, const int iMaxRange)
	{
		int iRandomNumber;
 		iRandomNumber = ((rand() % (iMaxRange - iMinRange + 1) ) + iMinRange);
		return static_cast<float> (iRandomNumber);
	}
}