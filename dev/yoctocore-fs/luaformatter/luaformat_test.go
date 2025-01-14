package luaformatter

import (
	"testing"
)

func BenchmarkLuaFormat(b *testing.B) {
	for i := 0; i < b.N; i++ {
		_, _ = Beautify("a =    3")
	}
}
func TestBeautify(t *testing.T) {
	tests := []struct {
		name    string
		code    string
		want    string
		wantErr bool
	}{
		{
			name: "Test 1",
			code: "a =    3",
			want: "a = 3",
		},
		{
			name: "Test 2",
			code: "a =    13;belle = 4",
			want: "a = 13\nbelle = 4",
		},
		{
			name: "Test 3",
			code: `S={1,2,3,4}
function x(y,  z)
	return y + z
end`,
			want: `S = {
	1,
	2,
	3,
	4
}
function x(y, z)
	return y + z
end`,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := Beautify(tt.code)
			if (err != nil) != tt.wantErr {
				t.Errorf("Beautify() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if got != tt.want {
				t.Errorf("Beautify() got: %v, want: %v", got, tt.want)
			}
		})
	}
}

func TestMinify(t *testing.T) {
	tests := []struct {
		name    string
		code    string
		want    string
		wantErr bool
	}{
		{
			name: "Test 1",
			code: "a = 3",
			want: "a=3",
		},
		{
			name: "Test 2",
			code: `a = 13
belle = 4
function  ok(hello, world)
		return hello  + world
		end`,
			want: "a=13;belle=4;function ok(hello,world)return hello+world end",
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := Minify(tt.code)
			if (err != nil) != tt.wantErr {
				t.Errorf("Minify() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if got != tt.want {
				t.Errorf("Minify() got: %v, want: %v", got, tt.want)
			}
		})
	}
}
