#ifndef QR_HXX
#define QR_HXX

extern "C" {

#include <math.h>
#include <stddef.h>
#include <stdint.h>

}

#include <BootKit/Vendor/Shared/base.h>
#include <BootKit/Vendor/Shared/bit.h>

#include <BootKit/Vendor/QrPrelude.hxx>
#include <BootKit/Support.hxx>

#include <Modules/CoreCG/FbRenderer.hxx>
#include <CompilerKit/Detail.hxx>

/// @note the QR code is still code 128, it utilizes the same concept of having it's own character set.

namespace qr
{
	inline uint8_t min_poly =
					   0b11101, /* Minimal polynomial x^8 + x^4 + x^3 + x^2 + 1 */
		generator = 0b10;		/* Generator of Galois field */

	/// @brief galois finite field multiplication.
	inline uint8_t gf_mul(uint8_t a, uint8_t b)
	{
		uint8_t res = 0;

		for (; b; b >>= 1)
		{
			if (b & 1)
				res ^= a;
			if (a & 0x80)
				a = (a << 1) ^ min_poly;
			else
				a <<= 1;
		}

		return res;
	}

	// Size of Ecc block with respect to level and version. 0 version is for
	// padding.
	constexpr int ECC_CODEWORDS_PER_BLOCK[4][41] = {
		{0, 7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26,
		 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30,
		 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
		{0, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22,
		 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28,
		 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},
		{0, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24,
		 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30,
		 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
		{0, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22,
		 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30,
		 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
	};

	// Number of Ecc blocks with respect to level and version. 0 version is for
	// padding.
	constexpr int N_ECC_BLOCKS[4][41] = {
		{0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4,
		 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12,
		 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},
		{0, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5, 5, 8, 9,
		 9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25,
		 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},
		{0, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8, 8, 10, 12,
		 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34,
		 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},
		{0, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16,
		 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40,
		 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},
	};

	// Positions of central modules of alignment patterns according to version. 0
	// version is for padding.
	constexpr int ALIGN_POS[41][7] = {
		{},
		{0},
		{6, 18},
		{6, 22},
		{6, 26},
		{6, 30},
		{6, 34},
		{6, 22, 38},
		{6, 24, 42},
		{6, 26, 46},
		{6, 28, 50},
		{6, 30, 54},
		{6, 32, 58},
		{6, 34, 62},
		{6, 26, 46, 66},
		{6, 26, 48, 70},
		{6, 26, 50, 74},
		{6, 30, 54, 78},
		{6, 30, 56, 82},
		{6, 30, 58, 86},
		{6, 34, 62, 90},
		{6, 28, 50, 72, 94},
		{6, 26, 50, 74, 98},
		{6, 30, 54, 78, 102},
		{6, 28, 54, 80, 106},
		{6, 32, 58, 84, 110},
		{6, 30, 58, 86, 114},
		{6, 34, 62, 90, 118},
		{6, 26, 50, 74, 98, 122},
		{6, 30, 54, 78, 102, 126},
		{6, 26, 52, 78, 104, 130},
		{6, 30, 56, 82, 108, 134},
		{6, 34, 60, 86, 112, 138},
		{6, 30, 58, 86, 114, 142},
		{6, 34, 62, 90, 118, 146},
		{6, 30, 54, 78, 102, 126, 150},
		{6, 24, 50, 76, 102, 128, 154},
		{6, 28, 54, 80, 106, 132, 158},
		{6, 32, 58, 84, 110, 136, 162},
		{6, 26, 54, 82, 110, 138, 166},
		{6, 30, 58, 86, 114, 142, 170},
	};

	// Return n-th bit of arr starting from MSB.
	constexpr uint8_t get_bit_r(uint8_t* arr, int n)
	{
		return (arr[n >> 3] >> (7 - (n & 7))) & 1;
	}

	// Add up to 16 bits to arr. Data starts from MSB as well as each byte of an
	// array.
	constexpr void add_bits(uint16_t data, int n, uint8_t* arr, size_t& pos)
	{
		while (n--)
		{
			arr[pos >> 3] |= ((data >> n) & 1) << (7 - (pos & 7));
			++pos;
		}
	}

	// Translate char to alphanumeric encoding value,
	constexpr int alphanumeric(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';

		if (c >= 'A' && c <= 'Z')
			return c - 'A' + 10;

		switch (c)
		{
		case ' ':
			return 36;
		case '$':
			return 37;
		case '%':
			return 38;
		case '*':
			return 39;
		case '+':
			return 40;
		case '-':
			return 41;
		case '.':
			return 42;
		case '/':
			return 43;
		case ':':
			return 44;
		}
		return -1;
	}

	// Check if string can be encoded in alphanumeric mode.
	constexpr bool is_alphanumeric(const char* str, size_t len)
	{
		for (size_t i = 0; i < len; ++i)
			if (alphanumeric(str[i]) == -1)
				return false;
		return true;
	}

	// Check if string can be encoded in numeric mode.
	constexpr bool is_numeric(const char* str, size_t len)
	{
		for (size_t i = 0; i < len; ++i)
			if (str[i] < '0' || str[i] > '9')
				return false;
		return true;
	}

	// Check if string can be encoded in kanji mode.
	constexpr bool is_kanji(const char* str, size_t len)
	{
		for (size_t i = 0; i < len; i += 2)
		{
			uint16_t val = uint16_t(str[i]) | (uint16_t(str[i + 1]) << 8);
			if (val < 0x8140 || val > 0xebbf || (val > 0x9ffc && val < 0xe040))
				return false;
		}
		return true;
	}

	// Reed-Solomon Ecc generator polynomial for the given degree.
	inline void gf_gen_poly(int degree, uint8_t* poly)
	{
		SetMem(poly, 0, degree);

		uint8_t root = poly[degree - 1] = 1;

		for (int i = 0; i < degree; ++i)
		{
			for (int j = 0; j < degree - 1; ++j)
				poly[j] = gf_mul(poly[j], root) ^ poly[j + 1];
			poly[degree - 1] = gf_mul(poly[degree - 1], root);
			root			 = (root << 1) ^ ((root >> 7) * 0x11d);
		}
	}

	// Polynomial division if Galois Field.
	inline void gf_poly_div(uint8_t* dividend, size_t len, uint8_t* divisor, int degree, uint8_t* result)
	{
		SetMem(result, 0, degree);

		for (size_t i = 0; i < len; ++i)
		{
			uint8_t factor = dividend[i] ^ result[0];
			MoveMem(&result[0], &result[1], degree - 1);
			result[degree - 1] = 0;
			for (int j = 0; j < degree; ++j)
				result[j] ^= gf_mul(divisor[j], factor);
		}
	}

	enum Ecc
	{
		L,
		M,
		Q,
		H,
	};

	enum Mode
	{
		M_NUMERIC,
		M_ALPHANUMERIC,
		M_BYTE,
		M_KANJI,
	};

	// Select appropriate encoding mode for string.
	constexpr Mode select_mode(const char* str, size_t len)
	{
		if (is_numeric(str, len))
			return M_NUMERIC;
		if (is_alphanumeric(str, len))
			return M_ALPHANUMERIC;
		if (is_kanji(str, len))
			return M_KANJI;
		return M_BYTE;
	}

	// Return size of Character Control Indicator in bits for given version and
	// mode.
	constexpr int cci(int ver, Mode mode)
	{
		constexpr int cnt[4][3] = {
			{10, 12, 14},
			{9, 11, 13},
			{8, 16, 16},
			{8, 10, 12},
		};
		if (ver < 10)
			return cnt[mode][0];
		if (ver < 27)
			return cnt[mode][1];
		return cnt[mode][2];
	}

	template <int V>
	struct Qr
	{
	private:
		friend class QrDelegate;
		bool draw(int x, int y) noexcept;

	public:
		constexpr auto side_size() const
		{
			return SIDE;
		}

		bool module(int x, int y);
		bool encode(const char* str, size_t len, Ecc ecc, int mask = -1);

	private:
		bool encode_data(const char* data, size_t len, Ecc ecc, uint8_t* out);
		void encode_ecc(uint8_t* data, Ecc ecc, uint8_t* out);

		void add_data(uint8_t* data, uint8_t* patterns);
		void add_patterns();
		void add_version();
		void add_format(Ecc ecc, int mask);
		void reserve_patterns(uint8_t* out);

		template <bool Black>
		void draw_rect(int y, int x, int height, int width, uint8_t* out);
		template <bool Black>
		void draw_bound(int y, int x, int height, int width, uint8_t* out);

		template <bool Horizontal>
		int	 rule_1_3_score();
		int	 penalty_score();
		int	 select_mask(Ecc ecc, uint8_t* patterns);
		void apply_mask(int mask, uint8_t* patterns);

	private:
		static_assert(V >= 1 && V <= 40, "invalid version");
		static constexpr int SIDE		  = 17 + V * 4;
		static constexpr int N_BITS		  = SIDE * SIDE;
		static constexpr int N_ALIGN	  = V == 1 ? 0 : V / 7 + 2;
		static constexpr int N_ALIGN_BITS = V > 1 ? (N_ALIGN* N_ALIGN - 3) * 25 : 0;
		static constexpr int N_TIMING_BITS =
			(SIDE - 16) * 2 - (10 * (V > 1 ? N_ALIGN - 2 : 0));
		static constexpr int N_VER_BITS = V > 6 ? 36 : 0;
		static constexpr int N_DAT_BITS =
			N_BITS - (192 + N_ALIGN_BITS + N_TIMING_BITS + 31 + N_VER_BITS);
		static constexpr int N_BYTES = utl::bytes_in_bits(N_BITS);		   // Actual number of bytes_in_bits
																		   // required to store whole Qr code
		static constexpr int N_DAT_BYTES = utl::bytes_in_bits(N_DAT_BITS); // Actual number of bytes_in_bits required to store
																		   // [data + ecc]
		static constexpr int N_DAT_CAPACITY =
			N_DAT_BITS >> 3; // Capacity of [data + ecc] without remainder bits
	private:
		/// @brief internal function to retrieve bit from a bitset.
		uint8_t get_arr_bit(uint8_t* arr, unsigned bit) const
		{
			return utl::get_arr_bit(arr, bit);
		}

		/// @brief internal function to set bit from a bitset.
		void set_arr_bit(uint8_t* arr, unsigned bit)
		{
			utl::set_arr_bit(arr, bit);
		}

		/// @brief internal function to clear bit from a bitset.
		void clr_arr_bit(uint8_t* arr, unsigned bit)
		{
			utl::clr_arr_bit(arr, bit);
		}

		uint8_t code[N_BYTES] = {};

		bool status = false;
	};

	// Get color of a module from left-to-right and top-to-bottom. Black is true.
	template <int V>
	bool Qr<V>::module(int x, int y)
	{
		return get_arr_bit(code, y * SIDE + x);
	}

	/// @brief draw a new QR code.
	template <int V>
	bool Qr<V>::draw(int whereX, int whereY) noexcept
	{
		if (!this->status)
			return false; // it may be invalid.

		CGInit();

		for (int y = 0; y < (this->side_size()); ++y)
		{
			for (int x = 0; x < (this->side_size()); ++x)
			{
				CGDrawInRegion(
					(this->module(x, y) ? RGB(00, 00, 00) : RGB(0xFF, 0xFF, 0xFF)),
					1, 1,
					x + whereX, y + whereY);
			}
		}

		CGFini();

		return true;
	}

	// Create Qr code with given error correction level. If mask == -1,
	// then best mask selected automatically. NOTE: Automatic mask is the
	// most expensive operation. Takes about 95 % of all computation time.
	template <int V>
	bool Qr<V>::encode(const char* str, size_t len, Ecc ecc, int mask)
	{
		uint8_t data[N_DAT_BYTES]		   = {};
		uint8_t data_with_ecc[N_DAT_BYTES] = {};
		uint8_t patterns[N_BYTES]		   = {};

		if (!encode_data(str, len, ecc, data))
		{
			return status = false;
		}

		encode_ecc(data, ecc, data_with_ecc);

		reserve_patterns(patterns);
		CopyMem(code, patterns, N_BYTES);

		add_data(data_with_ecc, patterns);
		add_patterns();
		add_version();

		mask = mask != -1 ? mask & 7 : select_mask(ecc, patterns);

		add_format(ecc, mask);
		apply_mask(mask, patterns);

		return status = true;
	}

	template <int V>
	bool Qr<V>::encode_data(const char* data, size_t len, Ecc ecc, uint8_t* out)
	{
		Mode mode = select_mode(data, len);

		size_t n_bits =
			(N_DAT_CAPACITY - ECC_CODEWORDS_PER_BLOCK[ecc][V] * N_ECC_BLOCKS[ecc][V])
			<< 3;
		size_t pos = 0;

		add_bits(1 << mode, 4, out, pos);
		add_bits(len, cci(V, mode), out, pos);

		if (mode == M_NUMERIC)
		{
			const size_t triplets	   = len / 3;
			const size_t triplets_size = triplets * 3;
			const size_t rem		   = len % 3;
			const size_t rem_bits	   = rem == 2 ? 7 : rem == 1 ? 4
																 : 0;
			const size_t total_size	   = 10 * triplets + rem_bits;

			if (pos + total_size > n_bits)
				return false;

			char buf[4] = {};

			for (size_t i = 0; i < triplets_size; i += 3)
			{
				buf[0] = data[i];
				buf[1] = data[i + 1];
				buf[2] = data[i + 2];

				uint16_t num = StringToLong(buf, NULL, 10);
				add_bits(num, 10, out, pos);
			}

			if (rem)
			{
				buf[0]	 = data[triplets_size];
				buf[1]	 = data[triplets_size + 1];
				buf[rem] = 0;

				uint16_t num = StringToLong(buf, NULL, 10);
				add_bits(num, rem_bits, out, pos);
			}
		}
		else if (mode == M_ALPHANUMERIC)
		{
			if (pos + 11 * (int(len & ~1ul) / 2) > n_bits)
				return false;

			for (int i = 0; i < int(len & ~1ul); i += 2)
			{
				uint16_t num = alphanumeric(data[i]) * 45 + alphanumeric(data[i + 1]);
				add_bits(num, 11, out, pos);
			}
			if (len & 1)
			{
				if (pos + 6 > n_bits)
					return false;

				add_bits(alphanumeric(data[len - 1]), 6, out, pos);
			}
		}
		else if (mode == M_BYTE)
		{
			if (pos + len * 8 > n_bits)
				return false;

			for (size_t i = 0; i < len; ++i)
				add_bits(data[i], 8, out, pos);
		}
		else
		{
			if (pos + 13 * (len / 2) > n_bits)
				return false;

			for (size_t i = 0; i < len; i += 2)
			{
				uint16_t val = ((uint8_t)data[i]) | (((uint8_t)data[i + 1]) << 8);
				uint16_t res = 0;
				val -= val < 0x9FFC ? 0x8140 : 0xC140;
				res += val & 0xff;
				res += (val >> 8) * 0xc0;
				add_bits(res, 13, out, pos);
			}
		}

		size_t padding = n_bits - pos;
		size_t i	   = 0;

		add_bits(0, padding > 4 ? 4 : padding, out, pos);

		if (pos & 7)
			add_bits(0, (8 - pos) & 7, out, pos);

		while (pos < n_bits)
			add_bits(++i & 1 ? 0xec : 0x11, 8, out, pos);

		return true;
	}

	template <int V>
	void Qr<V>::encode_ecc(uint8_t* data, Ecc ecc, uint8_t* out)
	{
		int n_blocks = N_ECC_BLOCKS[ecc][V];
		int ecc_len	 = ECC_CODEWORDS_PER_BLOCK[ecc][V];

		int n_data_bytes = N_DAT_CAPACITY - ecc_len * n_blocks;

		int n_short_blocks = n_blocks - N_DAT_CAPACITY % n_blocks;
		int short_len	   = N_DAT_CAPACITY / n_blocks - ecc_len;

		uint8_t gen_poly[30];
		uint8_t ecc_buf[30];

		gf_gen_poly(ecc_len, gen_poly);

		uint8_t* data_ptr = data;

		for (int i = 0; i < n_blocks; ++i)
		{
			int data_len = short_len;

			if (i >= n_short_blocks)
				++data_len;

			gf_poly_div(data_ptr, data_len, gen_poly, ecc_len, ecc_buf);

			for (int j = 0, k = i; j < data_len; ++j, k += n_blocks)
			{
				if (j == short_len)
					k -= n_short_blocks;
				out[k] = data_ptr[j];
			}
			for (int j = 0, k = n_data_bytes + i; j < ecc_len; ++j, k += n_blocks)
				out[k] = ecc_buf[j];

			data_ptr += data_len;
		}
	}

	template <int V>
	void Qr<V>::add_data(uint8_t* data, uint8_t* patterns)
	{
		int data_pos = 0;

		for (int x = SIDE - 1; x >= 1; x -= 2)
		{
			if (x == 6)
				x = 5;

			for (int i = 0; i < SIDE; ++i)
			{
				int y	  = !((x + 1) & 2) ? SIDE - 1 - i : i;
				int coord = y * SIDE + x;

				if (!get_arr_bit(patterns, coord))
				{
					if (get_bit_r(data, data_pos))
						set_arr_bit(code, coord);

					++data_pos;
				}

				if (!get_arr_bit(patterns, coord - 1))
				{
					if (get_bit_r(data, data_pos))
						set_arr_bit(code, coord - 1);

					++data_pos;
				}
			}
		}
	}

	template <int V>
	void Qr<V>::add_patterns()
	{
		// White bounds inside finders
		draw_bound<false>(1, 1, 5, 5, code);
		draw_bound<false>(1, SIDE - 6, 5, 5, code);
		draw_bound<false>(SIDE - 6, 1, 5, 5, code);

		// Finish alignment patterns
		for (int i = 0; i < N_ALIGN; ++i)
		{
			for (int j = 0; j < N_ALIGN; ++j)
			{
				if ((!i && !j) || (!i && j == N_ALIGN - 1) || (!j && i == N_ALIGN - 1))
					continue;
				draw_bound<false>(ALIGN_POS[V][i] - 1, ALIGN_POS[V][j] - 1, 3, 3, code);
			}
		}

		// Draw white separators
		draw_rect<false>(7, 0, 1, 8, code);
		draw_rect<false>(0, 7, 8, 1, code);
		draw_rect<false>(SIDE - 8, 0, 1, 8, code);
		draw_rect<false>(SIDE - 8, 7, 8, 1, code);
		draw_rect<false>(7, SIDE - 8, 1, 8, code);
		draw_rect<false>(0, SIDE - 8, 8, 1, code);

		// Perforate timing patterns
		for (int i = 7; i < SIDE - 7; i += 2)
		{
			clr_arr_bit(code, 6 * SIDE + i);
			clr_arr_bit(code, i * SIDE + 6);
		}
	}

	template <int V>
	void Qr<V>::add_version()
	{
		if (V < 7)
			return;

		uint32_t rem = V;

		for (uint8_t i = 0; i < 12; ++i)
			rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);

		uint32_t data = V << 12 | rem;

		for (int x = 0; x < 6; ++x)
		{
			for (int j = 0; j < 3; ++j)
			{
				int y = SIDE - 11 + j;

				bool black = (data >> (x * 3 + j)) & 1;

				if (!black)
				{
					clr_arr_bit(code, y * SIDE + x);
					clr_arr_bit(code, y + SIDE * x);
				}
			}
		}
	}

	template <int V>
	void Qr<V>::add_format(Ecc ecc, int mask)
	{
		int data = (ecc ^ 1) << 3 | mask;
		int rem	 = data;

		for (int i = 0; i < 10; i++)
			rem = (rem << 1) ^ ((rem >> 9) * 0b10100110111);

		int res = (data << 10 | rem) ^ 0b101010000010010;

		for (int i = 0; i < 6; ++i)
		{
			if ((res >> i) & 1)
			{
				set_arr_bit(code, SIDE * 8 + SIDE - 1 - i);
				set_arr_bit(code, SIDE * i + 8);
			}
			else
			{
				clr_arr_bit(code, SIDE * 8 + SIDE - 1 - i);
				clr_arr_bit(code, SIDE * i + 8);
			}
		}

		for (int i = 6; i < 8; ++i)
		{
			if ((res >> i) & 1)
			{
				set_arr_bit(code, SIDE * 8 + SIDE - 1 - i);
				set_arr_bit(code, SIDE * (i + 1) + 8);
			}
			else
			{
				clr_arr_bit(code, SIDE * 8 + SIDE - 1 - i);
				clr_arr_bit(code, SIDE * (i + 1) + 8);
			}
		}

		if ((res >> 8) & 1)
		{
			set_arr_bit(code, SIDE * 8 + 7);
			set_arr_bit(code, SIDE * (SIDE - 7) + 8);
		}
		else
		{
			clr_arr_bit(code, SIDE * 8 + 7);
			clr_arr_bit(code, SIDE * (SIDE - 7) + 8);
		}

		for (int i = 9, j = 5; i < 15; ++i, --j)
		{
			if ((res >> i) & 1)
			{
				set_arr_bit(code, SIDE * 8 + j);
				set_arr_bit(code, SIDE * (SIDE - 1 - j) + 8);
			}
			else
			{
				clr_arr_bit(code, SIDE * 8 + j);
				clr_arr_bit(code, SIDE * (SIDE - 1 - j) + 8);
			}
		}
	}

	template <int V>
	template <bool B>
	void Qr<V>::draw_rect(int y, int x, int height, int width, uint8_t* out)
	{
		if (B)
		{
			for (int dy = y * SIDE; dy < (y + height) * SIDE; dy += SIDE)
				for (int dx = x; dx < x + width; ++dx)
					set_arr_bit(out, dy + dx);
		}
		else
		{
			for (int dy = y * SIDE; dy < (y + height) * SIDE; dy += SIDE)
				for (int dx = x; dx < x + width; ++dx)
					clr_arr_bit(out, dy + dx);
		}
	}

	template <int V>
	template <bool B>
	void Qr<V>::draw_bound(int y, int x, int height, int width, uint8_t* out)
	{
		if (B)
		{
			for (int i = y * SIDE + x; i < y * SIDE + x + width; ++i)
				set_arr_bit(out, i);
			for (int i = (y + height - 1) * SIDE + x;
				 i < (y + height - 1) * SIDE + x + width; ++i)
				set_arr_bit(out, i);
			for (int i = (y + 1) * SIDE + x; i < (y + height - 1) * SIDE + x; i += SIDE)
				set_arr_bit(out, i);
			for (int i = (y + 1) * SIDE + x + width - 1;
				 i < (y + height - 1) * SIDE + x + width - 1; i += SIDE)
				set_arr_bit(out, i);
		}
		else
		{
			for (int i = y * SIDE + x; i < y * SIDE + x + width; ++i)
				clr_arr_bit(out, i);
			for (int i = (y + height - 1) * SIDE + x;
				 i < (y + height - 1) * SIDE + x + width; ++i)
				clr_arr_bit(out, i);
			for (int i = (y + 1) * SIDE + x; i < (y + height - 1) * SIDE + x; i += SIDE)
				clr_arr_bit(out, i);
			for (int i = (y + 1) * SIDE + x + width - 1;
				 i < (y + height - 1) * SIDE + x + width - 1; i += SIDE)
				clr_arr_bit(out, i);
		}
	}

	template <int V>
	void Qr<V>::reserve_patterns(uint8_t* out)
	{
		draw_rect<true>(0, 6, SIDE, 1, out);
		draw_rect<true>(6, 0, 1, SIDE, out);

		draw_rect<true>(0, 0, 9, 9, out);
		draw_rect<true>(SIDE - 8, 0, 8, 9, out);
		draw_rect<true>(0, SIDE - 8, 9, 8, out);

		for (int i = 0; i < N_ALIGN; ++i)
		{
			for (int j = 0; j < N_ALIGN; ++j)
			{
				if ((!i && !j) || (!i && j == N_ALIGN - 1) || (!j && i == N_ALIGN - 1))
					continue;
				draw_rect<true>(ALIGN_POS[V][i] - 2, ALIGN_POS[V][j] - 2, 5, 5, out);
			}
		}

		if (V >= 7)
		{
			draw_rect<true>(SIDE - 11, 0, 3, 6, out);
			draw_rect<true>(0, SIDE - 11, 6, 3, out);
		}
	}

	template <int V>
	template <bool H>
	int Qr<V>::rule_1_3_score()
	{
		constexpr int y_max	 = H ? N_BITS : SIDE;
		constexpr int x_max	 = H ? SIDE : N_BITS;
		constexpr int y_step = H ? SIDE : 1;
		constexpr int x_step = H ? 1 : SIDE;

		int res = 0;

		for (int y = 0; y < y_max; y += y_step)
		{
			bool color	= get_arr_bit(code, y);
			int	 finder = color;
			int	 cnt	= 1;
			for (int x = 1; x < x_max; x += x_step)
			{
				if (get_arr_bit(code, y + x) == color)
				{
					++cnt;
					if (cnt == 5)
						res += 3;
					if (cnt > 5)
						++res;
				}
				else
				{
					color = !color;
					cnt	  = 1;
				}
				// Finder-like
				finder = ((finder << 1) & 0x7ff) | color;
				if (x >= x_step * 10)
				{
					if (finder == 0x05d || finder == 0x5d0)
						res += 40;
				}
			}
		}
		return res;
	}

	template <int V>
	int Qr<V>::penalty_score()
	{
		int res = 0;

		res += rule_1_3_score<true>();
		res += rule_1_3_score<false>();

		for (int y = 0; y < N_BITS - SIDE; y += SIDE)
		{
			for (int x = 0; x < SIDE - 1; ++x)
			{
				bool c = get_arr_bit(code, y + x);

				if (c == get_arr_bit(code, y + x + 1) &&
					c == get_arr_bit(code, y + x + SIDE) &&
					c == get_arr_bit(code, y + x + SIDE + 1))
					res += 3;
			}
		}

		int black = 0;
		for (int y = 0; y < N_BITS; y += SIDE)
		{
			for (int x = 0; x < SIDE; ++x)
				black += get_arr_bit(code, y + x);
		}
		res += abs((black * 100) / N_BITS - 50) / 5 * 10;

		return res;
	}

	template <int V>
	int Qr<V>::select_mask(Ecc ecc, uint8_t* patterns)
	{
		unsigned min_score = -1;
		unsigned score	   = 0;
		uint8_t	 mask	   = 0;

		for (int i = 0; i < 8; ++i)
		{
			add_format(ecc, i);
			apply_mask(i, patterns);
			score = penalty_score();
			if (score < min_score)
			{
				mask	  = i;
				min_score = score;
			}
			apply_mask(i, patterns);
		}
		return mask;
	}

	template <int V>
	void Qr<V>::apply_mask(int mask, uint8_t* patterns)
	{
		for (int y = 0, dy = 0; y < SIDE; ++y, dy += SIDE)
		{
			for (int x = 0; x < SIDE; ++x)
			{
				int coord = dy + x;

				if (get_arr_bit(patterns, coord))
					continue;

				bool keep = true;

				switch (mask)
				{
				case 0:
					keep = (x + y) & 1;
					break;
				case 1:
					keep = y & 1;
					break;
				case 2:
					keep = x % 3;
					break;
				case 3:
					keep = (x + y) % 3;
					break;
				case 4:
					keep = (y / 2 + x / 3) & 1;
					break;
				case 5:
					keep = x * y % 2 + x * y % 3;
					break;
				case 6:
					keep = (x * y % 2 + x * y % 3) & 1;
					break;
				case 7:
					keep = ((x + y) % 2 + x * y % 3) & 1;
					break;
				}

				if (!keep)
				{
					if (get_arr_bit(code, coord))
						clr_arr_bit(code, coord);
					else
						set_arr_bit(code, coord);
				}
			}
		}
	}

	/// @brief QR code encoder class.
	class QrDelegate final
	{
	public:
		explicit QrDelegate() = default;
		~QrDelegate()		  = default;

		NEWOS_COPY_DEFAULT(QrDelegate);

		/// @brief Draw method delegate.
		template <int V>
		bool draw(Qr<V>& subject, int x, int y) noexcept
		{
			return subject.draw(x, y);
		}
	};
} // namespace qr

namespace Kernel::Qr
{
	using namespace qr;
} // namespace Kernel::Qr

#endif // QR_HXX
