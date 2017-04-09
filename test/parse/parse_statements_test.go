package test

import (
	. "github.com/onsi/ginkgo"
	. "github.com/onsi/ginkgo/extensions/table"

	"github.com/orange-lang/orange/ast"
)

var _ = Describe("Parsing Statements", func() {
	Describe("should be able to parse enums", func() {
		DescribeTable("one line", expectNode,
			Entry("one member", "enum Status { Pending }", &ast.EnumDecl{
				Name:    "Status",
				Members: []string{"Pending"},
			}),
			Entry("multiple members", "enum Status { Pending, Running, Finished }",
				&ast.EnumDecl{
					Name:    "Status",
					Members: []string{"Pending", "Running", "Finished"},
				},
			),
		)

		DescribeTable("multiple lines", expectNode,
			Entry("one member", `enum Status {
				Pending	
			}`, &ast.EnumDecl{
				Name:    "Status",
				Members: []string{"Pending"},
			}),

			Entry("multiple members", `enum Status {
				Pending, 
				Running,
				Finished
			}`, &ast.EnumDecl{
				Name:    "Status",
				Members: []string{"Pending", "Running", "Finished"},
			}),
		)

		DescribeTable("lines with extra whitespace", expectNode,
			Entry("one member", `enum Status {

				Pending	

			}`, &ast.EnumDecl{
				Name:    "Status",
				Members: []string{"Pending"},
			}),

			Entry("multiple members", `enum Status {

				Pending, 


				Running,

				Finished
				


			}`, &ast.EnumDecl{
				Name:    "Status",
				Members: []string{"Pending", "Running", "Finished"},
			}),
		)
	})

	DescribeTable("should be able to parse function calls", expectNode,
		Entry("no arguments", "func()", &ast.CallExpr{
			Object:    &ast.NamedIDExpr{Name: "func"},
			Arguments: []ast.Expression{},
		}),

		Entry("one argument", "func(1)", &ast.CallExpr{
			Object: &ast.NamedIDExpr{Name: "func"},
			Arguments: []ast.Expression{
				&ast.IntExpr{Value: 1, Size: 64},
			},
		}),

		Entry("multiple arguments", "func(1,2,3)", &ast.CallExpr{
			Object: &ast.NamedIDExpr{Name: "func"},
			Arguments: []ast.Expression{
				&ast.IntExpr{Value: 1, Size: 64},
				&ast.IntExpr{Value: 2, Size: 64},
				&ast.IntExpr{Value: 3, Size: 64},
			},
		}),
	)

	Describe("should be able to parse multiple statements", func() {
		It("separated by newlines", func() {
			expectNodes(`
				1
				2
				3
			`, []ast.Node{
				&ast.IntExpr{Value: 1, Size: 64},
				&ast.IntExpr{Value: 2, Size: 64},
				&ast.IntExpr{Value: 3, Size: 64},
			})
		})

		It("separted by semicolons", func() {
			expectNodes("1; 2; 3", []ast.Node{
				&ast.IntExpr{Value: 1, Size: 64},
				&ast.IntExpr{Value: 2, Size: 64},
				&ast.IntExpr{Value: 3, Size: 64},
			})
		})

		It("separated by newlines and semicolons", func() {
			expectNodes(`
				1;
				2;
				3
			`, []ast.Node{
				&ast.IntExpr{Value: 1, Size: 64},
				&ast.IntExpr{Value: 2, Size: 64},
				&ast.IntExpr{Value: 3, Size: 64},
			})
		})
	})

	DescribeTable("should be able to parse", expectNode,
		Entry("alias", "alias mystring = char[]", &ast.AliasDecl{
			Name: "mystring",
			Type: &ast.ArrayType{InnerType: &ast.CharType{}},
		}),
	)

	DescribeTable("should be able to handle variable declarations", expectNode,
		Entry("type and value", "var a: int = 5", &ast.VarDecl{
			Name:  "a",
			Type:  &ast.IntType{Size: 64, Signed: true},
			Value: &ast.IntExpr{Value: 5, Size: 64},
		}),

		Entry("type", "var a: int", &ast.VarDecl{
			Name:  "a",
			Type:  &ast.IntType{Size: 64, Signed: true},
			Value: nil,
		}),

		Entry("value", "var a = 5", &ast.VarDecl{
			Name:  "a",
			Type:  nil,
			Value: &ast.IntExpr{Value: 5, Size: 64},
		}),

		Entry("neither type nor value", "var a", &ast.VarDecl{
			Name:  "a",
			Type:  nil,
			Value: nil,
		}),
	)

	DescribeTable("should be able to handle package", expectNode,
		Entry("simple name", "package A", &ast.PackageDecl{
			Name: "A",
		}),

		Entry("nested name", "package A.B", &ast.PackageDecl{
			Name: "A.B",
		}),
	)

	DescribeTable("should be able to handle import", expectNode,
		Entry("simple name", "import A", &ast.ImportDecl{
			Name: "A",
		}),

		Entry("nested name", "import A.B", &ast.ImportDecl{
			Name: "A.B",
		}),
	)
})
