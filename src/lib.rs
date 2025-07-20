#![allow(dead_code, non_camel_case_types)]

use bstr::B;
use bstr::ByteSlice;

use icu_locale::Locale;
use icu_collator::{Collator, options::{CollatorOptions, Strength}};

use std::borrow::Cow;

use std::slice;
use std::os::raw::c_char;

type wchar_t = i32;
type size_t = usize;

fn strlen(s: *const c_char) -> size_t {
  let mut len: size_t = 0;
  let mut s = s;
  unsafe {
    while *s != 0 {
      s = s.offset(1);
      len += 1;
    }
  }
  len
}

fn wcslen(s: *const wchar_t) -> size_t {
  let mut len: size_t = 0;
  let mut s = s;
  unsafe {
    while *s != 0 {
      s = s.offset(1);
      len += 1;
    }
  }
  len
}

fn get_sortkey_u8<'a>(source: &[u8]) -> Cow<'a, [u8]> {
  let mut sortkey: Vec<u8> = Vec::new();

  // Get from struct
  let name = "en_US";
  let locale: Locale = Locale::try_from_str(&name.replace("_", "-")).expect("No such locale");

  let mut options = CollatorOptions::default();
  options.strength = Some(Strength::Quaternary);

  let collator = Collator::try_new(locale.into(), options).unwrap();

  collator.write_sort_key_utf8_to(source, &mut sortkey).unwrap();

  Cow::Owned(sortkey)
}

fn get_sortkey_u32<'a>(source: &[u32]) -> Cow<'a, [u32]> {
  let source: &[u8] = &source.iter().filter_map(|c| char::from_u32(*c)).collect::<String>().into_bytes();

  let sortkey: &[u8] = &get_sortkey_u8(source);
  let result: Vec<u32> = B(sortkey).chars().map(|c| c as u32).collect();

  Cow::Owned(result)
}

#[unsafe(no_mangle)]
extern "C" fn icu_strxfrm(
  dest: *mut c_char,
  src: *const c_char,
  n: size_t
) -> size_t {
  let source: &[u8] = unsafe { slice::from_raw_parts(src as *const u8, strlen(src)) };
  let sortkey: &[u8] = &get_sortkey_u8(source);

  if sortkey.len() < n && !sortkey.is_empty() {
    let destination: &mut [u8] = unsafe { slice::from_raw_parts_mut(dest as *mut u8, n) };

    destination[..sortkey.len()].copy_from_slice(sortkey);

    destination[sortkey.len()] = b'\0';
  }

  sortkey.len()
}

#[unsafe(no_mangle)]
extern "C" fn icu_wcsxfrm(
  dest: *mut wchar_t,
  src: *const wchar_t,
  n: size_t
) -> size_t {
  let source: &[u32] = unsafe { slice::from_raw_parts(src as *const u32, wcslen(src)) };
  let sortkey: &[u32] = &get_sortkey_u32(source);

  if sortkey.len() < n && !sortkey.is_empty() {
    let destination: &mut [u32] = unsafe { slice::from_raw_parts_mut(dest as *mut u32, n) };

    destination[..sortkey.len()].copy_from_slice(sortkey);

    destination[sortkey.len()] = '\0' as u32;
  }

  sortkey.len()
}
