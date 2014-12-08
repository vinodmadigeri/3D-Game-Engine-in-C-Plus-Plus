namespace Engine
{
	inline float GenerateRandomNumber(const float iMinRange, const float iMaxRange)
	{
		float iRandomNumber;
 		iRandomNumber = ((rand() % static_cast<int>(iMaxRange - iMinRange + 1) ) + iMinRange);
		return (iRandomNumber);
	}
}