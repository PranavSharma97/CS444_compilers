// JOOS1: PARSER_WEEDER,JOOS1_STATIC_FIELD_DECLARATION,PARSER_EXCEPTION
// JOOS2: PARSER_WEEDER,DISAMBIGUATION
//import a.b.c.a;
//import a.a.a.*;

public class test extends A{
	protected static int field = 23;
	public test() { }
	public static int test() { return test.field+100; }
}
