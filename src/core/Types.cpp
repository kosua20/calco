#include "core/Types.hpp"

std::string Value::toString(Format format) const {
	const bool internal = format == Format::INTERNAL;
	const bool rowMajor = (format & Format::MAJOR_MASK) == Format::MAJOR_ROW_FLAG;

	switch (type) {
		case BOOL:
			return internal ? (b ? "1" : "0") : (b ? "true" : "false");
		case INTEGER:
		{
			switch(format & Format::BASE_MASK){
				case Format::BASE_2_FLAG:
				{
					const unsigned long long ai = std::abs(i);
					const unsigned long long bitCount = (unsigned long long)((std::floor(std::log2(double(std::max(ai, 2ull))))) + 1);
					std::string out(bitCount, '0');
					for(unsigned long long bid = 0; bid < bitCount; ++bid){
						out[bitCount - bid - 1] = (ai & (1ull << bid)) ? '1' : '0';
					}
					return "0b" + out;
				}
				case Format::BASE_8_FLAG:
				{
					char tmpBuffer[256];
					snprintf(tmpBuffer, sizeof(tmpBuffer), "0o%llo", i);
					return std::string(tmpBuffer);
				}
				case Format::BASE_16_FLAG:
				{
					char tmpBuffer[256];
					snprintf(tmpBuffer, sizeof(tmpBuffer), "0x%llX", i);
					return std::string(tmpBuffer);
				}
				case Format::BASE_10_FLAG:
				default:
					return std::to_string(i);
			}
		}
		case FLOAT:
			return std::to_string(f);
		case VEC3:
		{
			std::string ms = internal ? "vec3( " : "| ";
			for(int cid = 0; cid < 3; ++cid){
				ms.append(std::to_string(v3[cid]));
				if(cid < 2){
					ms.append(", ");
				}
			}
			ms.append(internal ? " )" : " |");
			return ms;
		}
		case VEC4:
		{
			std::string ms = internal ? "vec4( " : "| ";
			for(int cid = 0; cid < 4; ++cid){
				ms.append(std::to_string(v4[cid]));
				if(cid < 3){
					ms.append(", ");
				}
			}
			ms.append(internal ? " )" : " |");
			return ms;
		}
		case MAT3:
		{
			std::string ms = internal ? "mat3( " : "| ";
			// Column major access.
			for(int cid = 0; cid < 3; ++cid){

				for(int cjd = 0; cjd < 3; ++cjd){
					const float val = rowMajor ? m3[cjd][cid] : m3[cid][cjd];
					ms.append(std::to_string(val));
					if(cjd < 2){
						ms.append(", ");
					}
				}

				if(cid < 2){
					ms.append(internal ? ", " : (" |\n| "));
				} else {
					ms.append(internal ? " )" : " |");
				}
			}
			return ms;
		}
		case MAT4:
		{
			std::string ms = internal ? "mat4( " : "| ";
			// Column major access.
			for(int cid = 0; cid < 4; ++cid){

				for(int cjd = 0; cjd < 4; ++cjd){
					const float val = rowMajor ? m4[cjd][cid] : m4[cid][cjd];
					ms.append(std::to_string(val));
					if(cjd < 3){
						ms.append(", ");
					}
				}

				if(cid < 3){
					ms.append(internal ? ", " : (" |\n| "));
				} else {
					ms.append(internal ? " )" : " |");
				}
			}
			return ms;
		}
		default:
			break;
	}
	return "unknown";
}

/** Type promotions;
 bool -> int, bool -> float, bool->vec3, bool->mat3, bool->vec4, bool->mat4
 int -> bool, int -> float, int->vec3, int->mat3, int->vec4, int->mat4
 float -> bool, float -> float, float->vec3, float->mat3, float->vec4, float->mat4

 vec3, vec4 and mat3, mat4 -> no promotion
 */
bool Value::convert(const Type& target, Value& outVal) const {

	if(target == type){
		outVal = *this;
		return true;
	}

	// String conversion is always possible.
	if(target == Value::STRING){
		outVal = toString(Format::INTERNAL);
		return true;
	}

	switch (type) {
		case BOOL:
			switch (target) {
				case INTEGER:
					outVal = (long long)(b ? 1 : 0);
					return true;
				case FLOAT:
					outVal = b ? 1.0 : 0.0;
					return true;
				case VEC3:
					outVal = glm::vec3(b ? 1.0f : 0.0f);
					return true;
				case VEC4:
					outVal = glm::vec4(b ? 1.0f : 0.0f);
					return true;
				case MAT3:
					outVal = glm::mat3(b ? 1.0f : 0.0f);
					return true;
				case MAT4:
					outVal = glm::mat4(b ? 1.0f : 0.0f);
					return true;
				default:
					break;
			}
			break;
		case INTEGER:
			switch (target) {
				case BOOL:
					outVal = (i != 0);
					return true;
				case FLOAT:
					outVal = double(i);
					return true;
				case VEC3:
					outVal = glm::vec3(float(i));
					return true;
				case MAT3:
					outVal = glm::mat3(float(i));
					return true;
				case VEC4:
					outVal = glm::vec4(float(i));
					return true;
				case MAT4:
					outVal = glm::mat4(float(i));
					return true;
				default:
					break;
			}
			break;
		case FLOAT:
			switch (target) {
				case BOOL:
					outVal = f != 0.0;
					return true;
				case INTEGER:
					outVal = (long long)(f);
					return true;
				case VEC3:
					outVal = glm::vec3(float(f));
					return true;
				case MAT3:
					outVal = glm::mat3(float(f));
					return true;
				case VEC4:
					outVal = glm::vec4(float(f));
					return true;
				case MAT4:
					outVal = glm::mat4(float(f));
					return true;
				default:
					break;
			}
			break;
		case VEC3:
		case MAT3:
		case VEC4:
		case MAT4:
		default:
			break;
	}
	return false;
}

Value Unary::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Binary::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Ternary::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Literal::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Member::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Variable::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value VariableDef::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}

Value FunctionDef::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}

Value FunctionVar::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}

Value FunctionCall::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}

Status TreeVisitor::getStatus() const {
	return !_failed ? Status(true) : Status(0, _failedMessage);
}

void TreeVisitor::registerError(const std::string& msg, Expression const * exp){
	if(!_failed){
		_failedMessage = msg;
		_failedExpression = exp;
		_failed = true;
	}
}

Status Expression::evaluate(TreeVisitor& visitor, Value& outValue){
	outValue = evaluate(visitor);
	return visitor.getStatus();
}
