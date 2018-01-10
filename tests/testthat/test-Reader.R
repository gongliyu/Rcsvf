context("Reader")

test_that("constructor", {
  reader <- Reader$new("data/simple.csv")
  reader$open("data/simple.csv")
  reader$open("data/mixnumchar.csv")
  reader2 <- Reader$new("data/mixnumchar.csv")
  reader2$open("data/mixnumchar.csv")
  reader$open("data/mixnumchar.csv")
  reader2$open("data/simple.csv")
  expect_error(Reader$new(tempfile()))
  expect_error(reader$open(tempfile()))
  expect_equal(2 * 2, 4)
})

test_that("properties", {
  reader <- Reader$new("data/simple.csv", verbose=FALSE)
  expect_identical(
    reader$properties(),
    list(sep=",", eol="\n", quote.rule="doubled", fill=TRUE,
         strip.white=TRUE, skip.blank.lines=TRUE,
         quote="\"", verbose=FALSE, nfields=as.integer(5),
         field.types=rep("integer",5),
         field.names=paste0("V",1:5)))
})

test_that("detect field types", {
  reader <- Reader$new("data/mixnumchar.csv")
  expect_identical(
    reader$properties()$field.types,
    c("character","double","integer"))
})

test_that("read", {
  reader <- Reader$new("data/simple.csv")
  expect_equivalent(
    reader$read(2),
    data.frame(V1=1:2,V2=1:2,V3=1:2,V4=1:2,V5=1:2))
  expect_equivalent(
    reader$read(100),
    data.frame(V1=3:7,V2=3:7,V3=3:7,V4=3:7,V5=3:7))
  reader$open("data/mixnumchar.csv")
  expect_equivalent(
    reader$read(100),
    data.frame(V1=c("hello","hello","hello","1.0","1"),
               V2=rep(1,5), V3=c(1,1,1,2,3),
               stringsAsFactors = FALSE))
})

test_that("chunk_uniformly", {
  reader <- Reader$new("data/simple.csv")
  offsets <- reader$chunk_uniformly(2, 6, 1)$offsets
  reader$open("data/simple.csv", begin.offset=offsets[1],
              end.offset=offsets[2])
  df1 <- reader$read(100)
  reader$open("data/simple.csv", begin.offset=offsets[2],
              end.offset=offsets[3])
  df2 <- reader$read(100)
  df <- rbind(df1, df2)
  expect_equivalent(
    df,
    data.frame(V1=1:7,V2=1:7,V3=1:7,V4=1:7,V5=1:7))
})
