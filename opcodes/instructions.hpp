struct Instruction
{
	const char *const name;
	uint8_t length;
	uint8_t cycles;
	uint8_t alt_cycles;
	const char *operand1;
	const char *operand2;
	bool cb_prefixed;
};

std::array<Instruction, 501> instructions
{
	{"NOP", 1, nullptr, nullptr, false},
	{"LD", 3, "BC", "d16", false},
	{"STOP", 1, "0", nullptr, false},
	{"LD", 3, "DE", "d16", false},
	{"LD", 1, "(DE)", "A", false},
	{"INC", 1, "DE", nullptr, false},
	{"INC", 1, "D", nullptr, false},
	{"DEC", 1, "D", nullptr, false},
	{"LD", 2, "D", "d8", false},
	{"RLA", 1, nullptr, nullptr, false},
	{"JR", 2, "r8", nullptr, false},
	{"ADD", 1, "HL", "DE", false},
	{"LD", 1, "A", "(DE)", false},
	{"DEC", 1, "DE", nullptr, false},
	{"INC", 1, "E", nullptr, false},
	{"DEC", 1, "E", nullptr, false},
	{"LD", 2, "E", "d8", false},
	{"RRA", 1, nullptr, nullptr, false},
	{"LD", 1, "(BC)", "A", false},
	{"JR", 2, "NZ", "r8", false},
	{"LD", 3, "HL", "d16", false},
	{"LD", 1, "(HL+)", "A", false},
	{"INC", 1, "HL", nullptr, false},
	{"INC", 1, "H", nullptr, false},
	{"DEC", 1, "H", nullptr, false},
	{"LD", 2, "H", "d8", false},
	{"DAA", 1, nullptr, nullptr, false},
	{"JR", 2, "Z", "r8", false},
	{"ADD", 1, "HL", "HL", false},
	{"LD", 1, "A", "(HL+)", false},
	{"DEC", 1, "HL", nullptr, false},
	{"INC", 1, "L", nullptr, false},
	{"DEC", 1, "L", nullptr, false},
	{"LD", 2, "L", "d8", false},
	{"CPL", 1, nullptr, nullptr, false},
	{"INC", 1, "BC", nullptr, false},
	{"JR", 2, "NC", "r8", false},
	{"LD", 3, "SP", "d16", false},
	{"LD", 1, "(HL-)", "A", false},
	{"INC", 1, "SP", nullptr, false},
	{"INC", 1, "(HL)", nullptr, false},
	{"DEC", 1, "(HL)", nullptr, false},
	{"LD", 2, "(HL)", "d8", false},
	{"SCF", 1, nullptr, nullptr, false},
	{"JR", 2, "C", "r8", false},
	{"ADD", 1, "HL", "SP", false},
	{"LD", 1, "A", "(HL-)", false},
	{"DEC", 1, "SP", nullptr, false},
	{"INC", 1, "A", nullptr, false},
	{"DEC", 1, "A", nullptr, false},
	{"LD", 2, "A", "d8", false},
	{"CCF", 1, nullptr, nullptr, false},
	{"INC", 1, "B", nullptr, false},
	{"LD", 1, "B", "B", false},
	{"LD", 1, "B", "C", false},
	{"LD", 1, "B", "D", false},
	{"LD", 1, "B", "E", false},
	{"LD", 1, "B", "H", false},
	{"LD", 1, "B", "L", false},
	{"LD", 1, "B", "(HL)", false},
	{"LD", 1, "B", "A", false},
	{"LD", 1, "C", "B", false},
	{"LD", 1, "C", "C", false},
	{"LD", 1, "C", "D", false},
	{"LD", 1, "C", "E", false},
	{"LD", 1, "C", "H", false},
	{"LD", 1, "C", "L", false},
	{"LD", 1, "C", "(HL)", false},
	{"LD", 1, "C", "A", false},
	{"DEC", 1, "B", nullptr, false},
	{"LD", 1, "D", "B", false},
	{"LD", 1, "D", "C", false},
	{"LD", 1, "D", "D", false},
	{"LD", 1, "D", "E", false},
	{"LD", 1, "D", "H", false},
	{"LD", 1, "D", "L", false},
	{"LD", 1, "D", "(HL)", false},
	{"LD", 1, "D", "A", false},
	{"LD", 1, "E", "B", false},
	{"LD", 1, "E", "C", false},
	{"LD", 1, "E", "D", false},
	{"LD", 1, "E", "E", false},
	{"LD", 1, "E", "H", false},
	{"LD", 1, "E", "L", false},
	{"LD", 1, "E", "(HL)", false},
	{"LD", 1, "E", "A", false},
	{"LD", 2, "B", "d8", false},
	{"LD", 1, "H", "B", false},
	{"LD", 1, "H", "C", false},
	{"LD", 1, "H", "D", false},
	{"LD", 1, "H", "E", false},
	{"LD", 1, "H", "H", false},
	{"LD", 1, "H", "L", false},
	{"LD", 1, "H", "(HL)", false},
	{"LD", 1, "H", "A", false},
	{"LD", 1, "L", "B", false},
	{"LD", 1, "L", "C", false},
	{"LD", 1, "L", "D", false},
	{"LD", 1, "L", "E", false},
	{"LD", 1, "L", "H", false},
	{"LD", 1, "L", "L", false},
	{"LD", 1, "L", "(HL)", false},
	{"LD", 1, "L", "A", false},
	{"RLCA", 1, nullptr, nullptr, false},
	{"LD", 1, "(HL)", "B", false},
	{"LD", 1, "(HL)", "C", false},
	{"LD", 1, "(HL)", "D", false},
	{"LD", 1, "(HL)", "E", false},
	{"LD", 1, "(HL)", "H", false},
	{"LD", 1, "(HL)", "L", false},
	{"HALT", 1, nullptr, nullptr, false},
	{"LD", 1, "(HL)", "A", false},
	{"LD", 1, "A", "B", false},
	{"LD", 1, "A", "C", false},
	{"LD", 1, "A", "D", false},
	{"LD", 1, "A", "E", false},
	{"LD", 1, "A", "H", false},
	{"LD", 1, "A", "L", false},
	{"LD", 1, "A", "(HL)", false},
	{"LD", 1, "A", "A", false},
	{"LD", 3, "(a16)", "SP", false},
	{"ADD", 1, "A", "B", false},
	{"ADD", 1, "A", "C", false},
	{"ADD", 1, "A", "D", false},
	{"ADD", 1, "A", "E", false},
	{"ADD", 1, "A", "H", false},
	{"ADD", 1, "A", "L", false},
	{"ADD", 1, "A", "(HL)", false},
	{"ADD", 1, "A", "A", false},
	{"ADC", 1, "A", "B", false},
	{"ADC", 1, "A", "C", false},
	{"ADC", 1, "A", "D", false},
	{"ADC", 1, "A", "E", false},
	{"ADC", 1, "A", "H", false},
	{"ADC", 1, "A", "L", false},
	{"ADC", 1, "A", "(HL)", false},
	{"ADC", 1, "A", "A", false},
	{"ADD", 1, "HL", "BC", false},
	{"SUB", 1, "B", nullptr, false},
	{"SUB", 1, "C", nullptr, false},
	{"SUB", 1, "D", nullptr, false},
	{"SUB", 1, "E", nullptr, false},
	{"SUB", 1, "H", nullptr, false},
	{"SUB", 1, "L", nullptr, false},
	{"SUB", 1, "(HL)", nullptr, false},
	{"SUB", 1, "A", nullptr, false},
	{"SBC", 1, "A", "B", false},
	{"SBC", 1, "A", "C", false},
	{"SBC", 1, "A", "D", false},
	{"SBC", 1, "A", "E", false},
	{"SBC", 1, "A", "H", false},
	{"SBC", 1, "A", "L", false},
	{"SBC", 1, "A", "(HL)", false},
	{"SBC", 1, "A", "A", false},
	{"LD", 1, "A", "(BC)", false},
	{"AND", 1, "B", nullptr, false},
	{"AND", 1, "C", nullptr, false},
	{"AND", 1, "D", nullptr, false},
	{"AND", 1, "E", nullptr, false},
	{"AND", 1, "H", nullptr, false},
	{"AND", 1, "L", nullptr, false},
	{"AND", 1, "(HL)", nullptr, false},
	{"AND", 1, "A", nullptr, false},
	{"XOR", 1, "B", nullptr, false},
	{"XOR", 1, "C", nullptr, false},
	{"XOR", 1, "D", nullptr, false},
	{"XOR", 1, "E", nullptr, false},
	{"XOR", 1, "H", nullptr, false},
	{"XOR", 1, "L", nullptr, false},
	{"XOR", 1, "(HL)", nullptr, false},
	{"XOR", 1, "A", nullptr, false},
	{"DEC", 1, "BC", nullptr, false},
	{"OR", 1, "B", nullptr, false},
	{"OR", 1, "C", nullptr, false},
	{"OR", 1, "D", nullptr, false},
	{"OR", 1, "E", nullptr, false},
	{"OR", 1, "H", nullptr, false},
	{"OR", 1, "L", nullptr, false},
	{"OR", 1, "(HL)", nullptr, false},
	{"OR", 1, "A", nullptr, false},
	{"CP", 1, "B", nullptr, false},
	{"CP", 1, "C", nullptr, false},
	{"CP", 1, "D", nullptr, false},
	{"CP", 1, "E", nullptr, false},
	{"CP", 1, "H", nullptr, false},
	{"CP", 1, "L", nullptr, false},
	{"CP", 1, "(HL)", nullptr, false},
	{"CP", 1, "A", nullptr, false},
	{"INC", 1, "C", nullptr, false},
	{"RET", 1, "NZ", nullptr, false},
	{"POP", 1, "BC", nullptr, false},
	{"JP", 3, "NZ", "a16", false},
	{"JP", 3, "a16", nullptr, false},
	{"CALL", 3, "NZ", "a16", false},
	{"PUSH", 1, "BC", nullptr, false},
	{"ADD", 2, "A", "d8", false},
	{"RST", 1, "00H", nullptr, false},
	{"RET", 1, "Z", nullptr, false},
	{"RET", 1, nullptr, nullptr, false},
	{"JP", 3, "Z", "a16", false},
	{"PREFIX", 1, "CB", nullptr, false},
	{"CALL", 3, "Z", "a16", false},
	{"CALL", 3, "a16", nullptr, false},
	{"ADC", 2, "A", "d8", false},
	{"RST", 1, "08H", nullptr, false},
	{"DEC", 1, "C", nullptr, false},
	{"RET", 1, "NC", nullptr, false},
	{"POP", 1, "DE", nullptr, false},
	{"JP", 3, "NC", "a16", false},
	{"CALL", 3, "NC", "a16", false},
	{"PUSH", 1, "DE", nullptr, false},
	{"SUB", 2, "d8", nullptr, false},
	{"RST", 1, "10H", nullptr, false},
	{"RET", 1, "C", nullptr, false},
	{"RETI", 1, nullptr, nullptr, false},
	{"JP", 3, "C", "a16", false},
	{"CALL", 3, "C", "a16", false},
	{"SBC", 2, "A", "d8", false},
	{"RST", 1, "18H", nullptr, false},
	{"LD", 2, "C", "d8", false},
	{"LDH", 2, "(a8)", "A", false},
	{"POP", 1, "HL", nullptr, false},
	{"LD", 1, "(C)", "A", false},
	{"PUSH", 1, "HL", nullptr, false},
	{"AND", 2, "d8", nullptr, false},
	{"RST", 1, "20H", nullptr, false},
	{"ADD", 2, "SP", "r8", false},
	{"JP", 1, "(HL)", nullptr, false},
	{"LD", 3, "(a16)", "A", false},
	{"XOR", 2, "d8", nullptr, false},
	{"RST", 1, "28H", nullptr, false},
	{"RRCA", 1, nullptr, nullptr, false},
	{"LDH", 2, "A", "(a8)", false},
	{"POP", 1, "AF", nullptr, false},
	{"LD", 1, "A", "(C)", false},
	{"DI", 1, nullptr, nullptr, false},
	{"PUSH", 1, "AF", nullptr, false},
	{"OR", 2, "d8", nullptr, false},
	{"RST", 1, "30H", nullptr, false},
	{"LD", 2, "HL", "SP+r8", false},
	{"LD", 1, "SP", "HL", false},
	{"LD", 3, "A", "(a16)", false},
	{"EI", 1, nullptr, nullptr, false},
	{"CP", 2, "d8", nullptr, false},
	{"RST", 1, "38H", nullptr, false},
	{"RLC", 2, "B", nullptr, true},
	{"RLC", 2, "C", nullptr, true},
	{"RL", 2, "B", nullptr, true},
	{"RL", 2, "C", nullptr, true},
	{"RL", 2, "D", nullptr, true},
	{"RL", 2, "E", nullptr, true},
	{"RL", 2, "H", nullptr, true},
	{"RL", 2, "L", nullptr, true},
	{"RL", 2, "(HL)", nullptr, true},
	{"RL", 2, "A", nullptr, true},
	{"RR", 2, "B", nullptr, true},
	{"RR", 2, "C", nullptr, true},
	{"RR", 2, "D", nullptr, true},
	{"RR", 2, "E", nullptr, true},
	{"RR", 2, "H", nullptr, true},
	{"RR", 2, "L", nullptr, true},
	{"RR", 2, "(HL)", nullptr, true},
	{"RR", 2, "A", nullptr, true},
	{"RLC", 2, "D", nullptr, true},
	{"SLA", 2, "B", nullptr, true},
	{"SLA", 2, "C", nullptr, true},
	{"SLA", 2, "D", nullptr, true},
	{"SLA", 2, "E", nullptr, true},
	{"SLA", 2, "H", nullptr, true},
	{"SLA", 2, "L", nullptr, true},
	{"SLA", 2, "(HL)", nullptr, true},
	{"SLA", 2, "A", nullptr, true},
	{"SRA", 2, "B", nullptr, true},
	{"SRA", 2, "C", nullptr, true},
	{"SRA", 2, "D", nullptr, true},
	{"SRA", 2, "E", nullptr, true},
	{"SRA", 2, "H", nullptr, true},
	{"SRA", 2, "L", nullptr, true},
	{"SRA", 2, "(HL)", nullptr, true},
	{"SRA", 2, "A", nullptr, true},
	{"RLC", 2, "E", nullptr, true},
	{"SWAP", 2, "B", nullptr, true},
	{"SWAP", 2, "C", nullptr, true},
	{"SWAP", 2, "D", nullptr, true},
	{"SWAP", 2, "E", nullptr, true},
	{"SWAP", 2, "H", nullptr, true},
	{"SWAP", 2, "L", nullptr, true},
	{"SWAP", 2, "(HL)", nullptr, true},
	{"SWAP", 2, "A", nullptr, true},
	{"SRL", 2, "B", nullptr, true},
	{"SRL", 2, "C", nullptr, true},
	{"SRL", 2, "D", nullptr, true},
	{"SRL", 2, "E", nullptr, true},
	{"SRL", 2, "H", nullptr, true},
	{"SRL", 2, "L", nullptr, true},
	{"SRL", 2, "(HL)", nullptr, true},
	{"SRL", 2, "A", nullptr, true},
	{"RLC", 2, "H", nullptr, true},
	{"BIT", 2, "0", "B", true},
	{"BIT", 2, "0", "C", true},
	{"BIT", 2, "0", "D", true},
	{"BIT", 2, "0", "E", true},
	{"BIT", 2, "0", "H", true},
	{"BIT", 2, "0", "L", true},
	{"BIT", 2, "0", "(HL)", true},
	{"BIT", 2, "0", "A", true},
	{"BIT", 2, "1", "B", true},
	{"BIT", 2, "1", "C", true},
	{"BIT", 2, "1", "D", true},
	{"BIT", 2, "1", "E", true},
	{"BIT", 2, "1", "H", true},
	{"BIT", 2, "1", "L", true},
	{"BIT", 2, "1", "(HL)", true},
	{"BIT", 2, "1", "A", true},
	{"RLC", 2, "L", nullptr, true},
	{"BIT", 2, "2", "B", true},
	{"BIT", 2, "2", "C", true},
	{"BIT", 2, "2", "D", true},
	{"BIT", 2, "2", "E", true},
	{"BIT", 2, "2", "H", true},
	{"BIT", 2, "2", "L", true},
	{"BIT", 2, "2", "(HL)", true},
	{"BIT", 2, "2", "A", true},
	{"BIT", 2, "3", "B", true},
	{"BIT", 2, "3", "C", true},
	{"BIT", 2, "3", "D", true},
	{"BIT", 2, "3", "E", true},
	{"BIT", 2, "3", "H", true},
	{"BIT", 2, "3", "L", true},
	{"BIT", 2, "3", "(HL)", true},
	{"BIT", 2, "3", "A", true},
	{"RLC", 2, "(HL)", nullptr, true},
	{"BIT", 2, "4", "B", true},
	{"BIT", 2, "4", "C", true},
	{"BIT", 2, "4", "D", true},
	{"BIT", 2, "4", "E", true},
	{"BIT", 2, "4", "H", true},
	{"BIT", 2, "4", "L", true},
	{"BIT", 2, "4", "(HL)", true},
	{"BIT", 2, "4", "A", true},
	{"BIT", 2, "5", "B", true},
	{"BIT", 2, "5", "C", true},
	{"BIT", 2, "5", "D", true},
	{"BIT", 2, "5", "E", true},
	{"BIT", 2, "5", "H", true},
	{"BIT", 2, "5", "L", true},
	{"BIT", 2, "5", "(HL)", true},
	{"BIT", 2, "5", "A", true},
	{"RLC", 2, "A", nullptr, true},
	{"BIT", 2, "6", "B", true},
	{"BIT", 2, "6", "C", true},
	{"BIT", 2, "6", "D", true},
	{"BIT", 2, "6", "E", true},
	{"BIT", 2, "6", "H", true},
	{"BIT", 2, "6", "L", true},
	{"BIT", 2, "6", "(HL)", true},
	{"BIT", 2, "6", "A", true},
	{"BIT", 2, "7", "B", true},
	{"BIT", 2, "7", "C", true},
	{"BIT", 2, "7", "D", true},
	{"BIT", 2, "7", "E", true},
	{"BIT", 2, "7", "H", true},
	{"BIT", 2, "7", "L", true},
	{"BIT", 2, "7", "(HL)", true},
	{"BIT", 2, "7", "A", true},
	{"RRC", 2, "B", nullptr, true},
	{"RES", 2, "0", "B", true},
	{"RES", 2, "0", "C", true},
	{"RES", 2, "0", "D", true},
	{"RES", 2, "0", "E", true},
	{"RES", 2, "0", "H", true},
	{"RES", 2, "0", "L", true},
	{"RES", 2, "0", "(HL)", true},
	{"RES", 2, "0", "A", true},
	{"RES", 2, "1", "B", true},
	{"RES", 2, "1", "C", true},
	{"RES", 2, "1", "D", true},
	{"RES", 2, "1", "E", true},
	{"RES", 2, "1", "H", true},
	{"RES", 2, "1", "L", true},
	{"RES", 2, "1", "(HL)", true},
	{"RES", 2, "1", "A", true},
	{"RRC", 2, "C", nullptr, true},
	{"RES", 2, "2", "B", true},
	{"RES", 2, "2", "C", true},
	{"RES", 2, "2", "D", true},
	{"RES", 2, "2", "E", true},
	{"RES", 2, "2", "H", true},
	{"RES", 2, "2", "L", true},
	{"RES", 2, "2", "(HL)", true},
	{"RES", 2, "2", "A", true},
	{"RES", 2, "3", "B", true},
	{"RES", 2, "3", "C", true},
	{"RES", 2, "3", "D", true},
	{"RES", 2, "3", "E", true},
	{"RES", 2, "3", "H", true},
	{"RES", 2, "3", "L", true},
	{"RES", 2, "3", "(HL)", true},
	{"RES", 2, "3", "A", true},
	{"RRC", 2, "D", nullptr, true},
	{"RES", 2, "4", "B", true},
	{"RES", 2, "4", "C", true},
	{"RES", 2, "4", "D", true},
	{"RES", 2, "4", "E", true},
	{"RES", 2, "4", "H", true},
	{"RES", 2, "4", "L", true},
	{"RES", 2, "4", "(HL)", true},
	{"RES", 2, "4", "A", true},
	{"RES", 2, "5", "B", true},
	{"RES", 2, "5", "C", true},
	{"RES", 2, "5", "D", true},
	{"RES", 2, "5", "E", true},
	{"RES", 2, "5", "H", true},
	{"RES", 2, "5", "L", true},
	{"RES", 2, "5", "(HL)", true},
	{"RES", 2, "5", "A", true},
	{"RRC", 2, "E", nullptr, true},
	{"RES", 2, "6", "B", true},
	{"RES", 2, "6", "C", true},
	{"RES", 2, "6", "D", true},
	{"RES", 2, "6", "E", true},
	{"RES", 2, "6", "H", true},
	{"RES", 2, "6", "L", true},
	{"RES", 2, "6", "(HL)", true},
	{"RES", 2, "6", "A", true},
	{"RES", 2, "7", "B", true},
	{"RES", 2, "7", "C", true},
	{"RES", 2, "7", "D", true},
	{"RES", 2, "7", "E", true},
	{"RES", 2, "7", "H", true},
	{"RES", 2, "7", "L", true},
	{"RES", 2, "7", "(HL)", true},
	{"RES", 2, "7", "A", true},
	{"RRC", 2, "H", nullptr, true},
	{"SET", 2, "0", "B", true},
	{"SET", 2, "0", "C", true},
	{"SET", 2, "0", "D", true},
	{"SET", 2, "0", "E", true},
	{"SET", 2, "0", "H", true},
	{"SET", 2, "0", "L", true},
	{"SET", 2, "0", "(HL)", true},
	{"SET", 2, "0", "A", true},
	{"SET", 2, "1", "B", true},
	{"SET", 2, "1", "C", true},
	{"SET", 2, "1", "D", true},
	{"SET", 2, "1", "E", true},
	{"SET", 2, "1", "H", true},
	{"SET", 2, "1", "L", true},
	{"SET", 2, "1", "(HL)", true},
	{"SET", 2, "1", "A", true},
	{"RRC", 2, "L", nullptr, true},
	{"SET", 2, "2", "B", true},
	{"SET", 2, "2", "C", true},
	{"SET", 2, "2", "D", true},
	{"SET", 2, "2", "E", true},
	{"SET", 2, "2", "H", true},
	{"SET", 2, "2", "L", true},
	{"SET", 2, "2", "(HL)", true},
	{"SET", 2, "2", "A", true},
	{"SET", 2, "3", "B", true},
	{"SET", 2, "3", "C", true},
	{"SET", 2, "3", "D", true},
	{"SET", 2, "3", "E", true},
	{"SET", 2, "3", "H", true},
	{"SET", 2, "3", "L", true},
	{"SET", 2, "3", "(HL)", true},
	{"SET", 2, "3", "A", true},
	{"RRC", 2, "(HL)", nullptr, true},
	{"SET", 2, "4", "B", true},
	{"SET", 2, "4", "C", true},
	{"SET", 2, "4", "D", true},
	{"SET", 2, "4", "E", true},
	{"SET", 2, "4", "H", true},
	{"SET", 2, "4", "L", true},
	{"SET", 2, "4", "(HL)", true},
	{"SET", 2, "4", "A", true},
	{"SET", 2, "5", "B", true},
	{"SET", 2, "5", "C", true},
	{"SET", 2, "5", "D", true},
	{"SET", 2, "5", "E", true},
	{"SET", 2, "5", "H", true},
	{"SET", 2, "5", "L", true},
	{"SET", 2, "5", "(HL)", true},
	{"SET", 2, "5", "A", true},
	{"RRC", 2, "A", nullptr, true},
	{"SET", 2, "6", "B", true},
	{"SET", 2, "6", "C", true},
	{"SET", 2, "6", "D", true},
	{"SET", 2, "6", "E", true},
	{"SET", 2, "6", "H", true},
	{"SET", 2, "6", "L", true},
	{"SET", 2, "6", "(HL)", true},
	{"SET", 2, "6", "A", true},
	{"SET", 2, "7", "B", true},
	{"SET", 2, "7", "C", true},
	{"SET", 2, "7", "D", true},
	{"SET", 2, "7", "E", true},
	{"SET", 2, "7", "H", true},
	{"SET", 2, "7", "L", true},
	{"SET", 2, "7", "(HL)", true},
	{"SET", 2, "7", "A", true},
};