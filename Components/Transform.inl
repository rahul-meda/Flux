
inline const Transform FxMul(const Transform& txA, const Transform& txB)
{
	Transform tx;
	tx.R = txA.R * txB.R;
	tx.position = (txA.R * txB.position) + txA.position;
	return tx;
}

inline const Transform FxMulT(const Transform& txA, const Transform& txB)
{
	Transform tx;
	tx.R = glm::transpose(txA.R) * txB.R;
	tx.position = glm::transpose(txA.R) * (txB.position - txA.position);
	return tx;
}