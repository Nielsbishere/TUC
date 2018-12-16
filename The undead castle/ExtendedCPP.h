#pragma once
template<typename O2, typename O1> bool instanceof(O1 &obj) {
	return dynamic_cast<O2*>(&obj);
}
template<typename O2, typename O1> bool instanceof(O1 *obj) {
	return dynamic_cast<O2*>(obj);
}